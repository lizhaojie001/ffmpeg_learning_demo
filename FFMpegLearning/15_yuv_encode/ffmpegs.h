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


struct YUVEncodeSpec
{
    char *filename;
    int w ;
    int h ;
    AVPixelFormat fmt;
    int fps;
};



class FFmpegs : public QObject
{
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void yuvEncode(YUVEncodeSpec &spec ,const char* outFile);
signals:

private:
    static int encode(AVCodecContext *ctx , AVFrame *frame ,AVPacket *pkt , QFile &outFile);

};

#endif // FFMPEGS_H
