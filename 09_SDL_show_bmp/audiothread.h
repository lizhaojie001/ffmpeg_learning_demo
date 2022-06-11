#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>




class AudioThread : public QThread
{
public:
    AudioThread(QObject * parent  = nullptr);
    ~AudioThread();
    void run() override;

};

#endif // AUDIOTHREAD_H
