#ifdef SEAD_ZSTD
#ifndef SEAD_ZSTDDECOMPRESSOR_H_
#define SEAD_ZSTDDECOMPRESSOR_H_

#include <zstd.h>

#include <container/seadTreeMap.h>
#include <resource/seadDecompressor.h>

namespace sead {
class ZSTDDecompressor : public Decompressor {
public:
    struct DecompContext {
        // TODO
        u8 stage;
        void* destp;
        s32 destCount;
        s32 _14;
        void* _18;
        void* _20;
        void* _28;
        ZSTD_DCtx_s* dctx;
    };

    explicit ZSTDDecompressor(SafeString& name);

    virtual ~ZSTDDecompressor();

private:
    using DictID = u32;
    struct DictNode {
        TreeMapNode<DictID> mMapNode;
        ZSTD_DDict* mDict;
    };

    // this should be sead::FixedTreeMap<DictID, ZSTD_DDict*, 32> but that has an extra pointer to the map at the end of each node...
    // seems there are probably two versions of the map since some other structs maintain that extra pointer
    void* mRootNode;
    FreeList mFreeList;
    int mCount;
    int mMax;
    DictNode mWork[32];
};
static_assert(sizeof(ZSTDDecompressor) == 0x590);
} // namespace sead

#endif // SEAD_ZSTDDECOMPRESSOR_H_
#endif // SEAD_ZSTD