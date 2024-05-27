#ifndef RWLOCKHELPER_H
#define RWLOCKHELPER_H

#include <QReadWriteLock>

class RwLockHelper
{
private:
    QReadWriteLock *rwLock;

public:
    RwLockHelper(QReadWriteLock *lock, bool read = true)
        : rwLock(lock)
    {
        if (read) {
            rwLock->lockForRead();
        } else {
            rwLock->lockForWrite();
        }
    }

    ~RwLockHelper() { rwLock->unlock(); }
};

#endif // RWLOCKHELPER_H
