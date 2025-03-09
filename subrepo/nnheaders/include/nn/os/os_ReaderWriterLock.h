#pragma once

#include <nn/os/os_ReaderWriterLockTypes.h>

namespace nn::os {

void InitializeReaderWriterLock(ReaderWriterLockType*);
void FinalizeReaderWriterLock(ReaderWriterLockType*);
bool TryAcquireReadLock(ReaderWriterLockType*);
void AcquireReadLock(ReaderWriterLockType*);
void ReleaseReadLock(ReaderWriterLockType*);
bool TryAcquireWriteLock(ReaderWriterLockType*);
void AcquireWriteLock(ReaderWriterLockType*);
void ReleaseWriteLock(ReaderWriterLockType*);
bool IsWriteLockHeldByCurrentThread(ReaderWriterLockType*);
bool IsReaderWriterLockOwnerThread(ReaderWriterLockType*);
bool IsReadLockHeld(ReaderWriterLockType*);

class ReaderWriterLock {
public:
    ReaderWriterLock() { InitializeReaderWriterLock(&m_Lock); }

    ~ReaderWriterLock();

    void LockReadLock();
    void LockWriteLock();
    void UnlockReadLock();
    void UnlockWriteLock();
    bool TryLockReadLock();
    bool TryLockWriteLock();
    bool IsReadLockHeld();
    bool IsWriteLockHeld();

    operator ReaderWriterLockType&();
    operator const ReaderWriterLockType&() const;
    ReaderWriterLockType* GetBase();

private:
    ReaderWriterLockType m_Lock;
};

} // namespace nn::os