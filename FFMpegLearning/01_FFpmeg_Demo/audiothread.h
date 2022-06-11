#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>




class AudioThread : public QThread
{
public:
    AudioThread(QObject * parent  = nullptr);
    ~AudioThread();
    void run() override;
    void stop();
private:
    bool _stop = false;
};

#endif // AUDIOTHREAD_H
