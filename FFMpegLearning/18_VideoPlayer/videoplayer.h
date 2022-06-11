#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include "qmutexcond.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


#define ERROR_BUF \
char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define END(func) \
if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        goto end; \
}

#define RET(func) \
if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        return ret; \
}



class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();
    enum PlayState
    {
        Playing,
        Paused,
        Stoped
    };
    void play();
    void pause();
    void stop();
    PlayState state();
    int64_t getDuration();

    bool isPlaying();
    void setFile(std::string &filename);
signals:
    void playStateChanged(VideoPlayer*p);
    void videoDuration(VideoPlayer*p);
    void playFailed(VideoPlayer *p);

private:


    int initDecoder(AVCodecContext**ctx, AVMediaType type , AVStream ** stream);

    void setState(PlayState state);

    void readFile();

/*************other *************/
private:
    PlayState _state = Paused;
//    char * _filename;
    std::string _filename;
    AVFormatContext * _fmtCtx = nullptr;
    int64_t _duration;

    int _is_planar = 0;
    int _per_sample_size = 0;
    int _per_sample_frame_size = 0;
    int _imageSize = 0;

/*************video *************/
private:
    AVStream * _vStream = nullptr;
    AVCodecContext * _vCodecCtx = nullptr;
    std::list<AVPacket> *_vPktList = nullptr;
    QMutexCond * _vMutex = nullptr;
    AVFrame * _vFrame = nullptr;

private:
    int initVideoInfo();
    void clearVideoPktList();
    void addVideoPkt(AVPacket &pkt);
    void readVideoPkt();


/*************audio *************/
private:
    AVStream * _aStream = nullptr;
    AVCodecContext * _aCodecCtx = nullptr;
    std::list<AVPacket> *_aPktList = nullptr;
    QMutexCond * _aMutex = nullptr;
    AVFrame * _aFrame = nullptr;

private:
    int initAudioInfo();
    void clearAudioPktList();
    void addAudioPkt(AVPacket &pkt);
    int readAudioPkt();
    //初始化SDL
    int initSDL();
    //重采样pcm数据
    void resamplePCM();

    static void onAudioCallback(void *userdata, Uint8 * stream,
                        int len);
    void doAudioCallback(Uint8 * stream,
                         int len);

};

#endif // VIDEOPLAYER_H
