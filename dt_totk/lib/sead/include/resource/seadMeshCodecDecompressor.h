#ifdef SEAD_ZSTD
#ifndef SEAD_MESHCODECDECOMPRESSOR_H_
#define SEAD_MESHCODECDECOMPRESSOR_H_

#include <resource/seadDecompressor.h>
#include <thread/seadLightSemaphore.h>

namespace sead {

// unsure if this is a sead struct or not
struct MeshCodecDecompContext {
    int _00;
    int readSize;
    void* outputBuffer;
    LightSemaphore _10;
    LightSemaphore _50;
    int _90[2];
    size_t _98;
    char _A0[0x10];
    size_t _B0;
    u8 _B8;
};

class MeshCodecDecompressor : public Decompressor {
public:
    virtual ~MeshCodecDecompressor();

    struct DecompContext {
        // TODO
        char _00[0x8];
        void* outputBuffer;
        char _10[0x18];
    };

private:
    MeshCodecDecompContext mMeshCodecContext;
};

} // namespace sead

#endif // SEAD_MESHCODECDECOMPRESSOR_H_
#endif // SEAD_ZSTD