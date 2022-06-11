#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

class PlayThread : public QThread
{
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();

  void  run() override;
};

#endif // PLAYTHREAD_H
