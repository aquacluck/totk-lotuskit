// log to debug console, typically only visible in emulators
sys::debugLog("hello debug"); // svcOutputDebugString syscall

// log through websocket (and debug log too)
Logger::logText("hello websocket", "/optional/ns/arg");

// AngelScript's `string format(fmt, ...)` uses `{}`
Logger::logText(format("{} {}", 420.69, "wikked"));

// print over the game **on this frame only** once
// (by default in drawlist 0 where ActorWatcher+HexDump go)
// using \n is required to align any calls that come after
TextWriter::print(0, "hello for 1/30 second \n");

// toasts keep printing after one call until expiration:
// - they don't belong to any numbered drawlist
// - they can't be repositioned
// - you can't stack too many toasts -- overflow is completely dropped
TextWriter::toast(30, "hello for 30 actual frames \n");

// drawlists 0-3 are used by builtin features, but reusing this is fine!
// it's always a balance of not letting text squish overlap, fitting enough
// chars, stopping layout jitter across frames, etc legibility decisions.
// try to print a consistent number of lines+spacing to avoid jitter.

// drawlists 4-7 are "free", but text will begin from the top left,
// overlapping ActorWatcher, and you can't change this
// TODO [script] cursor+scale drawnode bindings
TextWriter::print(4, format("{} {}\n", 420.69, "wikked"));

// AngelScript doesn't expose real pointers, this is a uint64 typedef
ptr_t somewhere = 0x3331337; // pointers are fake

// 4 lines dumped+drawn each frame (sizes must be multiples of 0x10 or 16)
// dump and draw sizes should usually be the same
HexDump::assignSlotAbsolute(0, sys::mainOffset()+somewhere, 0x40, 0x40);

// stop dumping targeted region, but keep drawing the last dumped contents
HexDump::pauseSlot(0);

// stop all dumping+drawing for slot 0
HexDump::clearSlot(0);

// see globals.cpp for all shapes+types
PrimitiveDrawer::drawSphere4x8(0, Player.pos, 5, Color4f(0, 1, 0, 0.1), Color4f(0, 0, 1, 1));

