#!/usr/bin/env python3
import asyncio
import base64
import hashlib
import json
import os
import re
import socket
from dataclasses import dataclass
from weakref import WeakSet
import urllib.parse

# see misc/scripts/logger-consumer.py for alt+simpler inits

class SocketAddrPresets:
    LOCALHOST = "127.0.0.1" # this machine where we do da python3
    ANYHOST = "0.0.0.0"
    SWITCHNET_AMSHOST = "10.0.0.3" # switch on lan
    # Listening on a specific net interface / veth / etc:
    # I have a second nic attached to an offline router + docked switch, with this address statically leased.
    # Then I point ryujinx at this interface too, so we can bind/call/etc to this one address for both switch+ryu usage, and isolate the targets from incoming lan noise if they're listening.
    # afaik an emu escape could still bind or talk to whatever it wants, ryu interface picker isn't a sandbox at that level.
    SWITCHNET_LOCALHOST = "10.0.0.2"


@dataclass
class BackendSocketConfig:
    bind_addr: str = SocketAddrPresets.SWITCHNET_LOCALHOST
    bind_port: int = 7072
    #msg_size: int = 0x500

    def __str__(self):
        return f"{self.bind_addr}:{self.bind_port}"


@dataclass
class FrontendSocketConfig:
    # WARNING don't bind this to ANYHOST or any interface you don't trust. Leave it as localhost, potential for host RCE with emulators
    bind_addr: str = SocketAddrPresets.LOCALHOST
    bind_port: int = 7073

    def __str__(self):
        return f"{self.bind_addr}:{self.bind_port}"


class BackendProxy:
    def __init__(self, socket_config: BackendSocketConfig):
        self.socket_config = socket_config
        self.command_send_queue = asyncio.Queue()
        self.mod_log_queue = asyncio.Queue()

    async def run_server(self):
        print(f"[backend.proxy] Listening for mod on {self.socket_config}...")
        server = await asyncio.start_server(self.handle_new_mod_connection, self.socket_config.bind_addr, self.socket_config.bind_port)
        async with server:
            await server.serve_forever()

    async def handle_new_mod_connection(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        ipc_inflight_commands = {} # uniq_id -> BackendCommandRecord
        self.command_send_queue = asyncio.Queue() # direct commands to this mod connection now, prevent any lingering connections from consuming/forwarding the active queue
        await asyncio.gather( # run concurrent
            BackendProxy.run_command_sender(self.command_send_queue, writer, ipc_inflight_commands), # forward commands to the mod
            BackendProxy.run_dgram_consumer(reader, ipc_inflight_commands, self.mod_log_queue), # receives+routes logs, data, command responses from the mod
        )
        writer.close()

    @staticmethod
    async def run_command_sender(command_send_queue: asyncio.Queue, writer: asyncio.StreamWriter, ipc_inflight_commands: dict):
        while True:
            command: BackendCommandRecord = await command_send_queue.get()
            ipc_inflight_commands[command.uniq_id] = command
            query: bytes = command.http_query # b"GET /favicon.ico HTTP/1.0\r\n\r\n"
            print(f"[backend.command] {query.decode().strip()}", flush=True)
            writer.write(query)
            await writer.drain()

    @staticmethod
    async def run_dgram_consumer(reader: asyncio.StreamReader, ipc_inflight_commands: dict, mod_log_queue: asyncio.Queue):
        DGRAM_OVERHEAD = 26 + 32; # XXX from cpp
        DGRAM_PAYLOAD_START = DGRAM_OVERHEAD - 3 - 1 # leading [0x   trailing ]

        # [0x00000000, "ns", 0x0000, payload] <-> struct dgram_resp { u32 uniq_id, char[32] namespace, u16 payload_len, char* payload };
        # the backend callsite emits a json literal payload, then it's printf %s all the way to raw http.
        #
        # BackendProxy ignores response headers and looks for this message array.
        # this way it's mostly forward-compatible to a real http implementation without doing any of that work.
        # similarly we dont need a cpp json impl or threads yet, but they fit in for free once we have them

        while True:
            try:
                # [0x%08x, \"%32s\", 0x%04x, %s]
                await reader.readuntil(b"[0x")

                dgram_meta: str = (await reader.readexactly(DGRAM_PAYLOAD_START)).decode()
                uniq_id = int(dgram_meta[:8], 16)
                ns: str = dgram_meta[11:11+32].strip()
                payload_len = int(dgram_meta[48:52], 16)
                payload: str = (await reader.readexactly(payload_len)).decode() # ignore the trailing ]
                #payload_json = json.loads(payload) # XXX unused json parse

            except (asyncio.exceptions.IncompleteReadError, BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
                break
            except json.decoder.JSONDecodeError as e:
                print(f"[backend.proxy][ERROR] malformed dgram, meta: {dgram_meta} payload: {payload}", flush=True)
                continue # usually the json is indeed bad (because that's done by hand rn) and the lengths are ok, so just move on to next message

            if ns in ("DEFAULT_TEXT", "SAVESTATE", "COMMAND", "BACKEND_CONFIG"):
                print(f"[{ns}] {payload}", flush=True) # TODO use svcOutputDebugString_log_ns_passlist filter? but that incurs json parse

            if uniq_id in ipc_inflight_commands:
                # Resolve ongoing command calls over http
                command: BackendCommandRecord = ipc_inflight_commands[uniq_id]
                del ipc_inflight_commands[uniq_id] # http_payload waiters hold remaining refs
                command.http_payload.set_result(payload) # resolve Future
                del command # unnecessary but this dict could pile up any leaks

            else:
                # Reassemble+forward logs over ws, FIXME apparently hex int literals aren't valid json
                #mod_log_queue.put_nowait(("[0x" + dgram_meta + payload + "]").encode())
                mod_log_queue.put_nowait((f"""[{uniq_id}, "{ns}", {payload}]""").encode())


class BackendCommandRecord:
    # a unit of frontend <-> backend "ipc", and represents a single roundtrip command invokation to the mod.
    # the browser-facing http connection will be stalled while this is in flight (fine since it's all xhr/fetch stuff where the browser will juggle remote sockets)

    def __init__(self, uniq_id: int, http_query: bytes, ns: str="COMMAND"):
        self.uniq_id = uniq_id
        self.ns = ns
        self.http_query: bytes = http_query
        self.http_payload: str = asyncio.Future() # .set_result()


class WSJankFrame:
    @classmethod
    def make_simple_server_text(cls, message: any = "balls") -> bytes:
        # no masking key, no extension data (for server)
        # https://datatracker.ietf.org/doc/html/rfc6455#section-5.2

        # frame type byte, + 0x80 = message FIN
        # 01 = text frame (2=bin, 8=close)

        # size byte (mask=0 for server). Oversizing is rejected by firefox, so we can't just ignore the small type and only send medium
        # 00 - 7d = 1 - 125B      -- 7 bit payload len (small)
        # 7e      = 126B - 64KB   -- next 2B are payload len (medium)
        # 7f      = >64KB         -- next 8B are payload len (large)

        if type(message) is str:
            message: bytes = message.encode()

        mlen = len(message)
        if mlen <= 125:
            return b"\x81"     + mlen.to_bytes(1, byteorder="big") + message
        elif mlen <= 65535:
            return b"\x81\x7e" + mlen.to_bytes(2, byteorder="big") + message
        else:
            return b"\x81\x7f" + mlen.to_bytes(8, byteorder="big") + message


class HTTPJankResponse:
    headers: bytes
    content: bytes
    code: str

    def to_wire(self) -> bytes:
        return self.headers + self.content

    #async def to_writer(self, writer: asyncio.StreamWriter, req_verb_line: str=""):
    #    writer.write(self.to_wire())
    #    await writer.drain()

    @classmethod
    def make_ws_upgrade_resp(cls, ws_key: bytes) -> "HTTPJankResponse":
        # https://datatracker.ietf.org/doc/html/rfc6455#section-4.2.2
        WS_MAGIC = b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
        accept_key: str = base64.b64encode(hashlib.sha1(ws_key + WS_MAGIC).digest()).decode()

        obj = cls()
        obj.content = b""
        obj.code = "101 Switching Protocols"
        obj.headers = f"HTTP/1.1 {obj.code}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: {accept_key}\r\n\r\n".encode()

        return obj # once this is sent, server may send() freely

    @classmethod
    def make_simple(cls, resp_content: any="balls", proto: str="HTTP/1.0", code: str="200 OK", content_type: str = None, do_content_length=True, do_cache_control=None) -> "HTTPJankResponse":
        if type(resp_content) is str:
            # python doesn't like bytes interpolation, so we have to concat them manually, so lets coalesce to bytes first
            resp_content: bytes = resp_content.encode()
        elif type(resp_content) is not bytes:
            resp_content: bytes = str(resp_content).encode()

        if do_cache_control is not None:
            raise NotImplementedError("idk")

        content_length: str = f"Content-Length: {len(resp_content)}\r\n" if do_content_length else ""
        content_type: str = f"Content-Type: {content_type}\r\n" if content_type else ""
        http_resp_headers: bytes = f"{proto} {code}\r\n{content_type}{content_length}\r\n".encode()

        obj = cls()
        obj.headers = http_resp_headers
        obj.content = resp_content
        obj.code = code

        return obj

    @classmethod
    def make_200(cls, **kwargs) -> "HTTPJankResponse":
        return cls.make_simple(code="200 OK", **kwargs)

    @classmethod
    def make_404(cls, **kwargs) -> "HTTPJankResponse":
        return cls.make_simple(code="404 Not Found", **kwargs)

    @classmethod
    async def make_read_asset_by_filename_unsafe(cls, asset_filename="", **kwargs) -> "HTTPJankResponse":
        # TODO use the async? or not
        # XXX only run frontend from this or parent dir, or chdir to __file__ at startup or something. this is pretty bad
        base = "" if os.getcwd().endswith("frontend") else "frontend/"
        try:
            #print(f"[frontend.http] opening {base+asset_filename}")
            resp_content = open(base+asset_filename, "rb").read()
            content_type = "application/javascript" if asset_filename.endswith("js") else None
            return cls.make_200(resp_content=resp_content, content_type=content_type)
        except FileNotFoundError as e:
            # we expect the file to exist, this isn't user input
            print(f"[frontend.http][err] {cls}.make_read_asset_by_filename_unsafe can't load {asset_filename}: {e}")
            return cls.make_404(resp_content=b"")


class FrontendSocket:
    WS_RE: re.Pattern = re.compile('\r\nSec-WebSocket-Key: ([^\r]+)\r\n')

    def __init__(self, socket_config: FrontendSocketConfig, backend_proxy: BackendProxy):
        self.socket_config = socket_config
        self.backend_proxy = backend_proxy
        self.broadcastable_ws_writers = WeakSet() # active ws asyncio.StreamWriter

    async def run_server(self):
        print(f"[frontend.http] Serving web on {self.socket_config}...")
        server = await asyncio.start_server(self.handle_client, self.socket_config.bind_addr, self.socket_config.bind_port)

        async def _serve_loop():
            async with server:
                await server.serve_forever()

        await asyncio.gather(_serve_loop(), self.run_websocket_log_broadcaster())

    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        while True:
            #print(f"[frontend.http] Reading client req", flush=True)
            # we need to recv the whole request before browsers will recv, they dont like to be interrupted apparently
            try:
                maybe_http_req: str = (await reader.readuntil(b"\r\n\r\n")).decode().strip()
                #print(f"[frontend.http] We got a client req {maybe_http_req}", flush=True)

            except (asyncio.exceptions.IncompleteReadError, BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
                break


            # yield the rest of this connection to ws upgrade
            if ws_match := self.WS_RE.search(maybe_http_req):
                # Upgrade: websocket
                # Sec-WebSocket-Key: dqQbeTWqdnbqvbOClTcCDA==
                #breakpoint()

                key: bytes = ws_match.group(1).encode()
                http_resp = HTTPJankResponse.make_ws_upgrade_resp(key)
                writer.write(http_resp.to_wire())
                await writer.drain()
                await self.websocket_takeover_client_connection(reader, writer)
                return


            # reduce req down to its verb line
            http_verb_line = ""
            for maybe_http_verb in maybe_http_req.split("\r\n"):
                maybe_http_verb: str = maybe_http_verb.strip('\r\n')

                # totally ignore other headers, ignore req bodies, we only care about verb lines. ignore the dumb ones too
                looks_verby = maybe_http_verb.startswith("GET ") or maybe_http_verb.startswith("POST ")
                if not looks_verby:
                    continue

                # come on bro fuzz me bro itll be fun
                noproto = maybe_http_verb.rstrip('/0123. \r\n')
                looks_httpy = noproto.endswith(" HTTP")
                if not looks_httpy:
                    continue

                http_verb_line = noproto[:-5] # strip trailing proto stuff
                break

            if not http_verb_line:
                continue # no verb lines in this \r\n\r\n delimited header block


            #print(f"[frontend.http] We got a verb line {http_verb_line}", flush=True)
            http_resp: HTTPJankResponse = await self.handle_verb_line(http_verb_line)
            print(f"[frontend.http] {http_verb_line} -> {http_resp.code}", flush=True)
            writer.write(http_resp.to_wire())
            await writer.drain()
            break

        writer.close()


    async def handle_verb_line(self, http_verb_line: str) -> HTTPJankResponse:
        if False:
            pass # TODO favicon, cors, backend proxy, whatever. you are the router now

        # proxy http requests for the /backend/ prefix almost verbatim
        elif http_verb_line.startswith("GET /backend/") or http_verb_line.startswith("POST /backend/"):

            raw_params = http_verb_line.split("?", 1)[1] # this is why stripping proto mattered
            uniq_id = int(raw_params[10:18], 16)
            http_verb_line_unquoted: bytes = urllib.parse.unquote(http_verb_line).encode()

            #print(f"sending {uniq_id}") #
            command = BackendCommandRecord(uniq_id, http_query=http_verb_line_unquoted + b" HTTP/1.0\r\n\r\n")
            self.backend_proxy.command_send_queue.put_nowait(command)

            await command.http_payload
            #print(f"got back {uniq_id}") #
            return HTTPJankResponse.make_200(resp_content=command.http_payload.result())
            #return HTTPJankResponse.make_200(resp_content="ok we queued it")


        # Static assets
        elif http_verb_line == "GET /":
            return await HTTPJankResponse.make_read_asset_by_filename_unsafe("assets/index.html")

        else: # resolve from a safe list of filenames
            for safe_asset in ("style.css", "fonts/SourceCodePro-ExtraLight.otf.woff2", "fonts/SourceCodePro-Medium.otf.woff2", "fonts/SourceCodePro-Black.otf.woff2", "js-vendor/htm@3.1.1.mjs", "js-vendor/preact-hooks@10.22.1.mjs", "js-vendor/preact-signals-core@1.7.0.mjs", "js-vendor/preact-signals@1.3.0.mjs", "js-vendor/preact@10.23.1.mjs"):
                if http_verb_line == f"GET /assets/{safe_asset}":
                    return await HTTPJankResponse.make_read_asset_by_filename_unsafe(f"assets/{safe_asset}")

        # Default 404
        return HTTPJankResponse.make_404(resp_content=b"")


    async def websocket_takeover_client_connection(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        print(f"[frontend.http] ws opened")

        # subscribe this client to self.run_websocket_log_broadcaster
        self.broadcastable_ws_writers.add(writer)

        while True:
            try:
                # TODO close, ping, pong?
                woo: bytes = await reader.readexactly(2)
                print(f"[frontend.ws] recv {woo}")

            except (asyncio.exceptions.IncompleteReadError, BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
                break

        # socket cleanup
        self.broadcastable_ws_writers.remove(writer)
        writer.close()

    async def run_websocket_log_broadcaster(self):
        while True:
            dgram_json: bytes = await self.backend_proxy.mod_log_queue.get()
            dgram_json_message: bytes = WSJankFrame.make_simple_server_text(dgram_json)

            for writer in self.broadcastable_ws_writers:
                writer.write(dgram_json_message)

            # we'll sometimes hit connection errors trying to write to closing websockets, just ignore, weakset means we'll stop trying this once it gets gc'd
            exc = await asyncio.gather(*[writer.drain() for writer in self.broadcastable_ws_writers], return_exceptions=True)


async def main():
    backend_proxy = BackendProxy(socket_config=BackendSocketConfig())
    frontend_socket = FrontendSocket(socket_config=FrontendSocketConfig(), backend_proxy=backend_proxy)
    while True:
        await asyncio.gather( # run concurrent
            backend_proxy.run_server(),
            frontend_socket.run_server(),
        )


if __name__ == "__main__":
    asyncio.run(main(), debug=True)
