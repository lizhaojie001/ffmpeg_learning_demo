#ifndef FFMPEGS_H
#define FFMPEGS_H
#include <QObject>
#include "CommonDefine.h"

class FFmpegs : public QObject
{
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void pcm2wav(WAVHeader &header , const char * pcmFilename , const char * wavFilename);
signals:

};

#endif // FFMPEGS_H
