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


struct ResampleAudioSpec
{
    std::string filename;
    int  chLayout;
    int sample_rate;
   AVSampleFormat  sample_format;
};

struct EncodeAudioSpec
{
    std::string filename;
    AVChannelLayout  chLayout;
    int sample_rate;
   AVSampleFormat  sample_format;
};

struct EncodeAudioOutSpec
{
    std::string filename;

    EncodeAudioOutSpec() {}
};


class FFmpegs : public QObject
{
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void aacDecode( EncodeAudioOutSpec &inSpec,EncodeAudioSpec &outspec);
signals:

private:
    static int decode(AVCodecContext *ctx , AVFrame *frame ,AVPacket *pkt , QFile &outFile);

};

#endif // FFMPEGS_H
