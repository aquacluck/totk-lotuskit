#pragma once

#include "nn/os/os_Mutex.h"

namespace nn::vfx2::detail {

class Mutex {
public:
    virtual void Lock();
    virtual void Unlock();

private:
    os::Mutex mMutex;

};

} // namespace nn::vfx2::detail