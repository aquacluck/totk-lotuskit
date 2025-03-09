#pragma once

#include <container/seadBuffer.h>
#include <heap/seadDisposer.h>
#include <hostio/seadHostIONode.h>
#include <prim/seadSafeString.h>
#include "common/aglResBinaryShaderArchive.h"
#include "common/aglResShaderArchive.h"

namespace agl {

class ResShaderProgram;
class ResShaderSource;
class ShaderProgram;
class ShaderProgramEdit;
class ShaderSource;

class ShaderProgramArchive : public sead::IDisposer {
public:
    ShaderProgramArchive();
    virtual ~ShaderProgramArchive();

    void destroy();
    void createWithOption(ResBinaryShaderArchive, ResShaderArchive, u32, sead::Heap*);
    void destroyResFile_();
    void initialize(ShaderProgramArchive*, s32, ResShaderProgram, sead::Heap*);
    void updateCompileInfo();
    void setUp();
    void setUp_(bool);
    void setUpFromObjectReflector(bool, bool);
    int searchShaderProgramIndex(const sead::SafeString&) const;
    sead::FormatFixedSafeString<1024> genMessage(sead::hostio::Context* context);
    void listenPropertyEvent(const sead::hostio::PropertyEvent* property_event);

private:
    ResBinaryShaderArchive mBinaryShaderArchive;
    ResShaderArchive mResShaderArchive;
    sead::Buffer<ShaderProgram> mShaderPrograms;
    char _40[0x48];
};
static_assert(sizeof(ShaderProgramArchive) == 0x88);

}  // namespace agl
