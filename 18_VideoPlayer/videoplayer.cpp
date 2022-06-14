#include "videoplayer.h"
#include <QDebug>
#include "thread"

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject{parent}
{

    //初始化子系统
    int ret = SDL_Init(SDL_INIT_AUDIO);
    if (ret != 0) {
        const char * err = SDL_GetError();
        qDebug() << err ;
        SDL_Quit();
        emit playFailed(this);
        return;
    }
    _vMutex = new QMutexCond();
    _aMutex = new QMutexCond();
    _aPktList = new std::list<AVPacket>();
    _vPktList = new std::list<AVPacket>();

    _file = new QFile("E:/out_ooo.pcm");
    _file->open(QFile::WriteOnly);


}



void VideoPlayer::free() {
    freeAudio();
    freeVideo();


    avcodec_close(_aCodecCtx);
    avcodec_close(_vCodecCtx);
    avformat_close_input(&_fmtCtx);
}

VideoPlayer::~VideoPlayer()
{
    delete _vMutex;
    delete _aMutex;
    delete _aPktList;
    delete _vPktList;
    SDL_Quit();
}


void VideoPlayer::play()
{
  if (isPlaying()) return;
  setState(Playing);
  qDebug() <<"文件名" << QString::fromStdString(_filename);
  std::thread  rg([this]() {
      qDebug() <<"子线程" <<"文件名" << QString::fromStdString(_filename);
          if (readFile() < 0) {
             qDebug() << "读取文件失败";
              emit playFailed(this);
          }
  });
  rg.detach();
}

void VideoPlayer::pause()
{
    if (!isPlaying()) return;
   setState(Paused);

}

void VideoPlayer::stop()
{
    setState(Stoped);
    free();
}

VideoPlayer::PlayState VideoPlayer::state()
{
    return _state;
}

int64_t VideoPlayer::getDuration()
{
    return  _duration;
}

bool VideoPlayer::isPlaying()
{
    return _state == Playing;
}

void VideoPlayer::setFile(std::string &filename)
{
    _filename = filename;
}

void VideoPlayer::setVolume(int volume)
{
    _volume = volume;
}

void VideoPlayer::setMute(bool mute)
{
    _mute = mute;
}



int VideoPlayer::initDecoder(AVCodecContext **ctx, AVMediaType type, AVStream **stream)
{
    //从流信息中获取数据
    int ret = av_find_best_stream(_fmtCtx,type,-1,-1,nullptr,0);
    RET(ret);
    int streamIndex = ret;
    //获取stream
    AVStream * st = _fmtCtx->streams[streamIndex];
    if(!st) {
        qDebug() << " _fmtCtx->streams[ret] error";
        return -1;
    }
    //找到对应的解码器
    const AVCodec * decoder = avcodec_find_decoder(st->codecpar->codec_id);
    if(!decoder) {
        qDebug() << "avcodec_find_decoder error";
        return -1;
    }
    //创建解码器上下文
    *ctx = avcodec_alloc_context3(decoder);
    if (!*ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        return -1;
    }
    // 从输入流中copy参数到解码器上下文
    ret = avcodec_parameters_to_context(*ctx,st->codecpar);
    RET(avcodec_parameters_to_context);

    //打开解码器
    ret = avcodec_open2(*ctx,decoder,nullptr);
    RET(avcodec_open2);

    *stream = st;


    return 0;

}

void VideoPlayer::setState(PlayState state)
{
    _state = state;
    emit playStateChanged(this);
}

int VideoPlayer::readFile()
{
    int ret = 0;
    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, _filename.c_str(), nullptr, nullptr);
    RET(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    RET(avformat_find_stream_info);

    // 打印流信息到控制台
    av_dump_format(_fmtCtx, 0, _filename.c_str(), 0);
    fflush(stderr);

    _duration = _fmtCtx->duration;
    emit videoDuration(this);


    //初始化视频模块
    bool initV = initVideoInfo() >= 0;
    bool initA = initAudioInfo() >= 0;
    //初始化音频模块
    if(!(initV||initA)) return -1;

//    std::thread([this]() {
//        readVideoPkt();
//    }).detach();

    //读取数据
    AVPacket pkt ;
    while (true) {
        ret = av_read_frame(_fmtCtx,&pkt) ;
        if (ret == 0) {
            if(pkt.stream_index == _aStream->index) {
                addAudioPkt(pkt);
            } else if(pkt.stream_index == _vStream->index) {
                addVideoPkt(pkt);
            }
        } else if(ret == AVERROR_EOF) {
            qDebug() << "读取到文件尾部";
            break;
        }else {
            continue;
        }
    }

    return 0;

}
