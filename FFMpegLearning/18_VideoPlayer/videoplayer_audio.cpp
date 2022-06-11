#include "videoplayer.h"
#include <QDebug>
static std::map<AVSampleFormat,SDL_AudioFormat> fmtMap= {
    {AV_SAMPLE_FMT_S16,AUDIO_S16LSB}
};

#define SUPPORT_SAMPLE_FMT AUDIO_S16LSB

int VideoPlayer::initAudioInfo()
{
    int ret = 0;
    ret = initDecoder(&_aCodecCtx,AVMEDIA_TYPE_AUDIO,&_aStream);
    RET(initDecoder);
    //计算单声道采样数据大小
    _per_sample_size = av_get_bytes_per_sample(_aCodecCtx->sample_fmt);
    _per_sample_frame_size = _per_sample_size * _aCodecCtx->ch_layout.nb_channels;
    //是否是planar
    _is_planar = av_sample_fmt_is_planar(_aCodecCtx->sample_fmt);
    qDebug() << "_is_planar:  " << _is_planar;


    //初始化SDL
    ret = initSDL();
    RET(initSDL);

    //初始化缓冲区
    _aFrame = av_frame_alloc();
    return 0;
}


void VideoPlayer::clearAudioPktList()
{
    _aMutex->lock();
    while (_aPktList->size() > 0) {
        AVPacket pkt = _aPktList->front();
        av_packet_unref(&pkt);
        _aPktList->pop_front();
    }
    _aMutex->unlock();
}

void VideoPlayer::addAudioPkt(AVPacket &pkt)
{
    _aMutex->lock();
    _aPktList->push_back(pkt);
    _aMutex->signal();
    _aMutex->unlock();
}

int VideoPlayer::readAudioPkt()
{
   //解码数据
    //给解码器发数据解码
    _aMutex->lock();
   if (_aPktList->empty()) {
        _aMutex->unlock();
        return 0;
   }

    AVPacket pkt = _aPktList->front();
    _aPktList->pop_front();
    int ret = avcodec_send_packet(_aCodecCtx,&pkt);
    if (ret < 0) {
        _aMutex->unlock();
        return -1;
    }

    av_packet_unref(&pkt);
    _aMutex->unlock();
    RET(avcodec_send_packet);

    ret = avcodec_receive_frame(_aCodecCtx,_aFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    }else RET(avcodec_receive_frame);

    int size = _aFrame->nb_samples
               * av_get_bytes_per_sample((AVSampleFormat)_aFrame->format)
               * _aFrame->channels;
    AVSampleFormat fmt = (AVSampleFormat)_aStream->codecpar->format;
   if (fmtMap[fmt] != SUPPORT_SAMPLE_FMT) { //判断是否需要重采样
        resamplePCM();
   }
   return size;

}

int VideoPlayer::initSDL()
{
    int ret = 0;

    SDL_AudioSpec spec;
    //采样率
    spec.freq = 44100;
    //采样格式
    spec.format = AUDIO_S16LSB;
    //声道数
    spec.channels = 2;
    //音频缓冲区样本数  值为2的n次幂
    spec.samples = 1024;
    //回调
    spec.callback = onAudioCallback;

    spec.userdata = this;
    //打开设备
    if(SDL_OpenAudio(&spec,nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        return -1;
    }

    //设置播放起始位置
    SDL_PauseAudio(0);

    return ret;
}

void VideoPlayer::resamplePCM()
{

}

void VideoPlayer::onAudioCallback(void *userdata, Uint8 *stream, int len)
{
    VideoPlayer * player = (VideoPlayer*)userdata;
    player->doAudioCallback(stream,len);
}

void VideoPlayer::doAudioCallback(Uint8 *stream, int len)
{
    int size = 0;
    while (true) {
         size = readAudioPkt();
        qDebug() << "doAudioCallback" << size;
//        if (size) {
//            SDL_MixAudio(stream,(Uint8 *)_aFrame->data,size,SDL_MIX_MAXVOLUME);
//            len -= size;
//            stream += size;
//        }
    }
}
