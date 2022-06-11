#ifndef QMUTEXCOND_H
#define QMUTEXCOND_H
#include "SDL2/SDL.h"
#include <QObject>

class QMutexCond : public QObject
{
    Q_OBJECT
public:
    explicit QMutexCond(QObject *parent = nullptr);

    void lock();
    void unlock();
    void signal();
    void wait();
    void broadcast();
signals:

private:
    SDL_mutex * _mutex = nullptr;
    SDL_cond * _cond = nullptr;

};

#endif // QMUTEXCOND_H
