#pragma once

#include <thread/seadCriticalSection.h>

namespace agl {

class CompactibleMemBlock;
class TextureCompactionJob;

struct TextureCompactionInfo {
    void* dest;
    void* work;
    size_t size;
    u16 offset;
    u8 _1E;
    u8 _1F;
    int texture_handle_index;
    TextureCompactionJob* job;
    char _30[8];
};

class TextureCompactionJob {
public:
    // TODO

private:
    u8 _00;
    u8 _01;
    u16 _02;
    CompactibleMemBlock* mMemBlock0;
    CompactibleMemBlock* mMemBlock1;
    u8 mStateFlags;
    u8 mIndex; // +1
    int mAlignment;
    u16 mOffset0;
    u16 mOffset1;
    size_t mDataSize;
    int mTextureHandleIndex;
    void* mDestBuffer;
    void* mWorkBuffer; // but not always?
    TextureCompactionInfo* mInfo;
    sead::CriticalSection mCS;
};

} // namespace agl