#include "thread/seadCriticalSection.h"

namespace sead
{
CriticalSection::CriticalSection()
{
    nn::os::InitializeMutex(&mCriticalSectionInner, true, 0);
}

CriticalSection::~CriticalSection()
{
    nn::os::FinalizeMutex(&mCriticalSectionInner);
}

void CriticalSection::lock()
{
    nn::os::LockMutex(&mCriticalSectionInner);
}

bool CriticalSection::tryLock()
{
    return nn::os::TryLockMutex(&mCriticalSectionInner);
}

void CriticalSection::unlock()
{
    nn::os::UnlockMutex(&mCriticalSectionInner);
}
}  // namespace sead
