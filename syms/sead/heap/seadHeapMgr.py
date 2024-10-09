from build_types import *
begin_sym_file(__file__)

# namespace sead { class HeapMgr { public: static HeapMgr* sInstancePtr; }; }
EXKING("_ZN4sead7HeapMgr12sInstancePtrE", {
    TOTK_100: 0x0463bea0,
    TOTK_110: 0x047193c0,
    TOTK_121: 0x04716ca0,
})

# sRootHeaps
EXKING("_ZN4sead7HeapMgr10sRootHeapsE", {
    TOTK_100: 0x0463be08,
    TOTK_110: 0x04719328,
    TOTK_121: 0x04716c08,
})

# findContainHeap
EXKING("_ZNK4sead7HeapMgr15findContainHeapEPKv", {
    TOTK_100: 0x00783ac4,
    TOTK_110: 0x007612b0,
    TOTK_121: 0x007fe124,
})

# createRootHeap_
EXKING("_ZN4sead7HeapMgr15createRootHeap_Ev", {
    TOTK_100: 0x00e9b7bc,
    TOTK_110: 0x00ebb084,
    TOTK_121: 0x00e7cdc4,
})
