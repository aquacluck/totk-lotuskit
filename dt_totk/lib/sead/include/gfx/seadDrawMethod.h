#pragma once

#include <heap/seadDisposer.h>
#include <prim/seadDelegate.h>
#include <prim/seadNamable.h>

namespace sead {

class DrawMethod : public IDisposer, public INamable {
public:
    // TODO

private:
    u32 _28[4];
    Delegate<DrawMethod> mDrawDelegate; // TODO figure out the actual type
};

} // namespace sead