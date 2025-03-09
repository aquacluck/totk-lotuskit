/**
 * @file Heap.h
 * @brief VFX heap implementation.
 */

#pragma once

#include <nn/os/os_Mutex.h>
#include <nn/types.h>

namespace nn::vfx2 {

class Heap {
public:
    virtual ~Heap();
    virtual void* Allocate(size_t size, int alignment);
    virtual void Free(void* ptr);

private:
    void* mHeap; // sead::Heap but this can't be depedent on sead so idk
};

class ResHeap : Heap {
public:
    ~ResHeap() override;
    void* Allocate(size_t size, int alignment) override;
    void Free(void* ptr) override;
    virtual void FreeWithSize(void* ptr, size_t size);

private:
    nn::os::Mutex mAllocatorMutex;
    void* _30;
    size_t mAllocSize;
    u64 mAllocCount;
};

}  // namespace nn::vfx2
