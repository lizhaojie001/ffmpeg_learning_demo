#ifndef FFMPEGS_H
#define FFMPEGS_H
#include <QObject>
#include <QDebug>
#include "CommonDefine.h"
#include <QFile>
extern "C" {
//#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}


struct DecodeVideoSpec
{
    char * filename;
    int  fps;
    int width;
    int height;
    AVPixelFormat  pix_fmt;
};



class FFmpegs : public QObject
{
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void decodeH264(const char * inFilename,DecodeVideoSpec &outSpec);
signals:

private:
    static int decode(AVCodecContext *ctx , AVFrame *frame ,AVPacket *pkt , QFile &outFile);

};

#endif // FFMPEGS_H
