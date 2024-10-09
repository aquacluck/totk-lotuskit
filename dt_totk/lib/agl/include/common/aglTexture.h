#pragma once

#include <container/seadListImpl.h>
#include <container/seadOffsetList.h>
#include <container/seadTList.h>
#include <nn/gfx/gfx_MemoryPool.h>
#include <nn/gfx/gfx_ResTextureData.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/util/util_ResDic.h>
#include "common/aglGPUMemBlock.h"

namespace agl {

class Texture;

struct TextureMemoryPool {
    nn::gfx::MemoryPool memory_pool;
    GPUMemBlock<u8> mem_block;
    nn::gfx::MemoryPool _160;
    void* memory;
    void* _288;
};

struct TextureInfo {
    TextureMemoryPool memory;
    nn::gfx::ResTextureData* p_res_tex_data;
    char _298[8];
    size_t required_mem_size;
    void* texture_data;
    char _2B0[0x10];
    sead::ListNode list_node;
    u8 _2D0;
    Texture* texture;
    char _2E0[8];
    nn::gfx::ResTextureData res_tex_data;
    char _388[0x10];
};

class Texture {
public:
    // TODO

private:
    u16 _00;
    u8 mState;
    u8 _03;
    int _04; // might be a reference count or something
    Texture* _08;
    int _10;
    int _14;
    void* mTextureDataRegion;
    uintptr_t mHeapPtrAndFlags; // agl::CompactibleHeap
    int mHandleIndex;
    int mTextureSize;
    sead::OffsetList<void> mListNode; // TODO figure out what this is a list of
    sead::TListNode<Texture*> mMgrListNode;
    nn::gfx::ResTextureData mResTextureData;
    u16 mNameLen;
    char mNameStorage[126]; // unsure of actual size
    void* mUserDataRegion;
    char _180;
    sead::ListNode mUnkTextureList; // seems to be a list of other textures but unsure
    TextureInfo* mTextureInfo;
    nn::gfx::ResTextureData* mTextureDataPtr;
    nn::util::ResDic mTextureInfoDict;
    char _1C0[0x10];
};

struct TextureHandle {
    u32 hash;
    const char* name;
    Texture* texture;
};

} // namespace agl