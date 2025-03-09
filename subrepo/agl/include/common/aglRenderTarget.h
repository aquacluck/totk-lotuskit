#pragma once

#include "common/aglTextureData.h"

namespace agl {

class RenderTargetColor : public TextureData {
public:
    RenderTargetColor();
    RenderTargetColor(const TextureData&, u32, u32);

    virtual ~RenderTargetColor();

    void onApplyTextureData_();
    void initRegs_(u32) const;

private:
    char _B8[0x18];

};

class RenderTargetDepth : public TextureData {
public:
    RenderTargetDepth();
    RenderTargetDepth(const TextureData&, u32, u32);

    virtual ~RenderTargetDepth();

    void onApplyTextureData_();
    void initRegs_(u32) const;

private:
    char _B8[0x18];
};

}  // namespace agl
