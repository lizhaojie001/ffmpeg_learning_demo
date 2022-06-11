#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>




class AudioThread : public QThread
{
public:
    AudioThread( void * winId, QObject * parent  = nullptr);
    ~AudioThread();
    void run() override;
private:
    void * _winId;
};

#endif // AUDIOTHREAD_H
