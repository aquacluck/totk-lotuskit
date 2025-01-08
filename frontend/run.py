#!/usr/bin/env python3

# serve frontend folder regardless of initial cwd
import os
os.chdir(os.path.dirname(__file__))

# mimetype patch https://stackoverflow.com/a/63167571
# fixed in 3.12+backports? https://bugs.python.org/issue31715
import http.server
HandlerClass = http.server.SimpleHTTPRequestHandler
HandlerClass.extensions_map['.js'] = 'text/javascript'
HandlerClass.extensions_map['.mjs'] = 'text/javascript'

# DualStackServer extracted from `python3 -m http.server`'s __main__ (inaccessible from here)
# https://github.com/python/cpython/blob/d6fa1d4beef2bf9d83048469667e0ba5f2b41068/Lib/http/server.py#L1280-L1326
import contextlib
import socket
class DualStackServer(http.server.ThreadingHTTPServer):
    def server_bind(self):
        # suppress exception when protocol is IPv4
        with contextlib.suppress(Exception):
            self.socket.setsockopt(
                socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)
        return super().server_bind()

    def finish_request(self, request, client_address):
        self.RequestHandlerClass(request, client_address, self,
                                 directory=os.getcwd())

# test() only makes a ThreadingHTTPServer by default, so 127.0.0.1 would be unserved in (extremely common) dualstack envs.
# It was a patch in https://github.com/python/cpython/commit/ee94bdb0598f9bc47d6a49e58fffc97aa617be96 kept unusable to module consumers.
# (inlining test() as well might be *nicer* now, but it's good to emphasize how unspecial+unoptimal this server is -- it doesn't matter)
http.server.test(HandlerClass, DualStackServer, port=7073)
