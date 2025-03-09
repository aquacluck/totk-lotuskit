#pragma once

#include <nvn/nvn.h>
#include <basis/seadTypes.h>

namespace agl::driver {

class NVNtexture_ {
public:
    // these are probably no longer accurate
    NVNtexture_();
    NVNtexture_(const NVNtexture_& other);
    ~NVNtexture_();

    void releaseTexture();
    void updateTexId_(s32 newID);
    NVNtexture_ operator=(const NVNtexture_& other);
    bool registerTexture(const NVNtexture*, const NVNtextureView*, const char*, bool);

    void setReference_() const;

private:
    s32 mTextureID;
    int _04[3];
};

}  // namespace agl::driver
