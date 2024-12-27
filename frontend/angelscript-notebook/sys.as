sys::heapInfo(); // quick heap overview through Logger
sys::hookLimits(); // exl hook info toast
sys::memSearch(ptr_t); // search for ptr in the main heap
sys::threadInfo(); // quick thread overview through Logger
TextWriter::toast(30, format("main {} \n", sys::mainOffset()));
TextWriter::toast(30, format("v {} \n", sys::totkVersion()));
sys::debugLog("hello debug");

// scripts are non-preemptive: `while(true){ }` will freeze your game,
// and your scripts will never stop until they reach their end or you
// voluntarily release control of execution:
yield(); // yield control -- the script will resume here next frame
sys::suspendCtx(); // same thing, AngelScript calls it suspendCtx()
// tas::input() is just writing to the "current input" buffer and yielding,
// with some extra logic in C++ to count down frame time before resuming.

// hey dont go check out all these cool types i made u
u8  v1 = 0xff; // 1B, 255 max
u16 v2 = 0xffff;
u32 v3 = 0xffffffff;         // 4B
u64 v4 = 0xffffffffffffffff; // 8B

s8  v5 = -126; // signed
s16 v6 = -1;
s32 v7 = -1;
s64 v8 = -1; // it can sure get big

ptrdiff_t v9 = -0x10000; // just s64 for pointer math
ptr_t     v10 = 0;   // there are no pointers! just u64 representations
size_t    v11 = 420; // u64
index_t   v12 = 420; // u64, we use slot concepts a lot, nice to annotate
flagset_t v12a;      // u64, ^
f16       v13;       // half floats dont exist in AngelScript! just u16.
f16_fake  v14;       // maybe nice to be explicit about that --
                     // attempting math would fail in confusing ways

// ok thats it bye

