#pragma once

#ifdef NNSDK
#include <nn/os.h>
#endif

#include "heap/seadDisposer.h"

namespace sead {
class Heap;

class LightSemaphore : public IDisposer {
public:
    LightSemaphore();
    explicit LightSemaphore(s32 initial_count);
    LightSemaphore(s32 initial_count, s32 max_count);

    explicit LightSemaphore(Heap* heap);
    LightSemaphore(Heap* heap, s32 initial_count);
    LightSemaphore(Heap* heap, s32 initial_count, s32 max_count);

    LightSemaphore(Heap* heap, HeapNullOption heap_null_option);
    LightSemaphore(Heap* heap, HeapNullOption heap_null_option, s32 initial_count);
    LightSemaphore(Heap* heap, HeapNullOption heap_null_option, s32 initial_count, s32 max_count);

    ~LightSemaphore() override;

    LightSemaphore(const LightSemaphore&) = delete;
    LightSemaphore& operator=(const LightSemaphore&) = delete;

    void initialize(s32 initial_count) { initialize(initial_count, initial_count); }
    void initialize(s32 initial_count, s32 max_count);
    void lock();
    bool tryLock();
    void unlock();

    bool try_lock() { return tryLock(); }

private:
    void setInitialized([[maybe_unused]] bool initialized) {
#ifdef SEAD_DEBUG
        mInitialized = initialized;
#endif
    }

#ifdef NNSDK
    nn::os::LightSemaphoreType mSemaphoreInner;
#else
#error "Unknown platform"
#endif

#ifdef SEAD_DEBUG
    bool mInitialized = false;
#endif
};
}  // namespace sead
