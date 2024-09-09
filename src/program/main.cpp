#include "lib.hpp"
#include "hooks/main.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nnMainHook::Install();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
