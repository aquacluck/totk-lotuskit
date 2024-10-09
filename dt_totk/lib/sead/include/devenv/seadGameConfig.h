#pragma once

#include "heap/seadDisposer.h"
#include "hostio/seadHostIONode.h"
#include "prim/seadSafeString.h"

namespace sead
{
class GameConfig : public hostio::Node
{
    GameConfig();
    virtual ~GameConfig();

    static const SafeString cNodeName;

protected:
    struct FileWriteCallback
    {
        virtual ~FileWriteCallback();
        virtual void save();
    };

private:
    SEAD_SINGLETON_DISPOSER(GameConfig)
};
}  // namespace sead
