#include "lib.hpp"
#include "hooks.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nnMainHook::Install(); // entry point in hooks.hpp
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
