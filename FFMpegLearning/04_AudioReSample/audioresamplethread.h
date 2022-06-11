#ifndef AUDIORESAMPLETHREAD_H
#define AUDIORESAMPLETHREAD_H

#include <QThread>

class AudioReSampleThread : public QThread
{
public:
    explicit AudioReSampleThread(QObject *parent = nullptr);
    ~AudioReSampleThread();
private:
    void run();
};

#endif // AUDIORESAMPLETHREAD_H
