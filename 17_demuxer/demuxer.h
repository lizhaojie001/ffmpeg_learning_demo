#ifndef DEMUXER_H
#define DEMUXER_H
#include <QObject>
#include <QFile>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

}

struct DecodeVideoSpec
{
    char * filename;
    int  fps;
    int width;
    int height;
    AVPixelFormat  pix_fmt;
};


struct DecodeAudioSpec
{
    char * filename;
    AVSampleFormat  sample_fmt;
    int channels;
    int sample_rate;
};



class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr);

    void demuxer(const char * inFilename,DecodeAudioSpec & aOut,DecodeVideoSpec & vOut);


private:
    int initVideoInfo();

    int initAudioInfo();

    int initDecoder(AVCodecContext**ctx, AVMediaType type , int * streamIndex);

    int decode(AVCodecContext * ctx, AVPacket *pkt, void (Demuxer::*func)());

    void writeAudio();
    void writeVideo();
signals:


private:

    AVFormatContext * _fmtCtx = nullptr;
    AVCodecContext * _vCodecCtx = nullptr;
    AVCodecContext * _aCodecCtx = nullptr;

    AVFrame * _frame = nullptr;

    int _is_planar = 0;
    int _per_sample_size = 0;
    int _per_sample_frame_size = 0;
    int _imageSize = 0;

    int _aStreamIndex = 0;
    int _vStreamIndex = 0;

    DecodeAudioSpec * _aOut;
    DecodeVideoSpec * _vOut;

    QFile _aFile;
    QFile _vFile;

    // 存放一帧解码图片的缓冲区
    uint8_t * _data[4] = {nullptr};
    int _lineSize[4] = {0};
};

#endif // DEMUXER_H
