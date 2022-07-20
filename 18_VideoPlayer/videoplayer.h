#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include "qmutexcond.h"
#include <QFile>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
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

    struct SwsVideoSpec
    {
        int width;
        int height;
        AVPixelFormat pix_fmt;
        int size;
    };

    void play();
    void pause();
    void stop();
    PlayState state();
    int getDuration();
    int getCurrent();
    void setTime(int secound);

    bool isPlaying();
    void setFile(std::string& filename);
    void setVolume(int volume);
    void setMute(bool mute);
signals:
    void playStateChanged(VideoPlayer*p);
    void videoDuration(VideoPlayer*p);
    void timeChanged(VideoPlayer*p);
    void playFailed(VideoPlayer *p);
    void playerVideoDeceoded(VideoPlayer*p, uint8_t * data, SwsVideoSpec &spec);
    void initFinish(VideoPlayer*);
private:


    int initDecoder(AVCodecContext**ctx, AVMediaType type , AVStream ** stream);

    void setState(PlayState state);

    int readFile();

    void free();

/*************other *************/
private:
    PlayState _state = Stoped;
//    char * _filename;
    std::string _filename;
    AVFormatContext * _fmtCtx = nullptr;
    int _duration;
    int seekTime = -1;
    int _is_planar = 0;
    int _per_sample_size = 0;
    int _per_sample_frame_size = 0;
    int _imageSize = 0;

    QFile *  _file = nullptr;

/*************video *************/
private:
    AVStream * _vStream = nullptr;
    AVCodecContext * _vCodecCtx = nullptr;
    std::list<AVPacket> *_vPktList = nullptr;
    QMutexCond * _vMutex = nullptr;

    //格式转换上下文
    SwsContext * _vSwsCtx = nullptr;
    //格式转换缓冲区
    AVFrame * _vSwsOutFrame = nullptr, * _vSwsInFrame = nullptr;


    SwsVideoSpec _vSwsOutSpec;

    double _vClock = -1;
    int  _vSeekTime = -1;
    bool _vCanFree = false;
private:
    int initVideoInfo();
    void clearVideoPktList();
    void addVideoPkt(AVPacket &pkt);
    void readVideoPkt();
    void freeVideo();
    //初始化格式转换上下文
    int initSws();


/*************audio *************/
private:
    AVStream * _aStream = nullptr;
    AVCodecContext * _aCodecCtx = nullptr;
    std::list<AVPacket> *_aPktList = nullptr;
    QMutexCond * _aMutex = nullptr;
    int _volume;
    bool _mute = false;
    struct SwrAudioSpec
    {
        int  chLayout;
        int sample_rate;
        AVSampleFormat  sample_format;
        int chs;
        int bytesPerSampleFrame;

    };
    SwrContext  *_SwrCtx;
    SwrAudioSpec _aSwrInSpec,_aSwrOutSpec;
    //重采样输出缓冲区
    AVFrame * _aSwrOutFrame = nullptr, * _aSwrInFrame = nullptr;

    //偏移量
    int _startOffsetIndex = 0;
    //重采样后大小
    int _swrOutSize = 0;

    //音频时钟
    double _aClock = 0;
    bool _aCanFree = false;

private:
    int initAudioInfo();
    void clearAudioPktList();
    void addAudioPkt(AVPacket &pkt);
    int readAudioPkt();
    //初始化SDL
    int initSDL();
    //重采样pcm数据
    int initSWR();
    int resamplePCM();
    void freeAudio();

    static void onAudioCallback(void *userdata, Uint8 * stream,
                        int len);
    void doAudioCallback(Uint8 * stream,
                         int len);


};

#endif // VIDEOPLAYER_H
