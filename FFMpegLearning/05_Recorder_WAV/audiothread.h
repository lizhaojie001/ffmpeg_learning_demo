#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>




class AudioThread : public QThread
{
    Q_OBJECT
public:
    AudioThread(QObject * parent  = nullptr);
    ~AudioThread();
    void run() override;
    void stop();

signals:
    void recoder_time_changed(unsigned long long);

private:
    bool _stop = false;

};

#endif // AUDIOTHREAD_H
