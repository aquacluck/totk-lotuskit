#pragma once

#include <cstdint>

#include <nn/os/detail/os_InternalCriticalSection.h>
#include "nn/os/detail/os_InternalConditionVariable.h"

namespace nn::os {

struct ThreadType;

struct ReaderWriterLockType {
    detail::InternalCriticalSectionStorage _mutex;
    int _readLockState;
    int _writeLockState;
    char _0C[12];
    ThreadType* _ownerThread;
    char _20[4];
    detail::InternalConditionVariableStorage _24;
    detail::InternalConditionVariableStorage _28;
};
static_assert(sizeof(ReaderWriterLockType) == 0x30);

} // namespace nn::os