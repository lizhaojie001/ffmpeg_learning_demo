#include "qmutexcond.h"

QMutexCond::QMutexCond(QObject *parent)
    : QObject{parent}
{
    _mutex = SDL_CreateMutex();
    _cond = SDL_CreateCond();
}

void QMutexCond::lock()
{
    SDL_LockMutex(_mutex);
}

void QMutexCond::unlock()
{
    SDL_UnlockMutex(_mutex);
}

void QMutexCond::signal()
{
    SDL_CondSignal(_cond);
}

void QMutexCond::wait()
{
    SDL_CondWait(_cond,_mutex);
}

void QMutexCond::broadcast()
{
    SDL_CondBroadcast(_cond);
}
