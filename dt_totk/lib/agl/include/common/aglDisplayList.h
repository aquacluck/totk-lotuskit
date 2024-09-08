#pragma once

#include <basis/seadTypes.h>
#include "common/aglGPUMemAddr.h"

namespace sead {
class Heap;
}

namespace agl {

class CommandBufferImpl;

// TODO: verify the methods
class DisplayListBase {
public:
    DisplayListBase();
    virtual ~DisplayListBase();

    void setControlMemory(void* memory, s32 size);
    void clear();
    void setBuffer(GPUMemAddr<u8> buffer, u64 size);
    void setValidSize(u64 size);
    void copyTo(DisplayListBase* other) const;
    void beginDisplayList();
    void endDisplayList();
    bool beginDisplayListBuffer(GPUMemAddr<u8> addr, u64 size, bool invalidate_cpu);
    void endDisplayListBuffer(sead::Heap* heap);
    void adjustValueSize();
    void invalidateCPUCache() const;
    void dump() const;
    bool suspend(void**);     // unused parameter
    void resume(void*, u64);  // unused parameters
    u64 calcRemainingSize();

private:
    GPUMemAddr<u8> mBuffer;
    u32 mUsedSize;
    u32 mUsedCommandMemorySize;
    char* mName;
    CommandBufferImpl* mCommandBuffer;
    void* mControlMemory;
    u32 mControlMemorySize;
    u32 mUsedControlMemory;
    void* mLastCommandList;
    u8 _50;
};

class DisplayList : DisplayListBase {
public:
    ~DisplayList() override;

private:
    char _58[0x208];
};

}  // namespace agl
