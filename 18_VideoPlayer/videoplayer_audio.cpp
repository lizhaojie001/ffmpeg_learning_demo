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



    //初始化重采样上下文
    ret = initSWR();
    RET(initSWR);
    //初始化SDL
    ret = initSDL();
    RET(initSDL);


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
    _aMutex->unlock();
    if (pkt.pts != AV_NOPTS_VALUE) {
        _aClock = av_q2d(_aStream->time_base) * pkt.pts;
        emit timeChanged(this);
    }

    int ret = avcodec_send_packet(_aCodecCtx,&pkt);

    RET(avcodec_send_packet)
    av_packet_unref(&pkt);

    ret = avcodec_receive_frame(_aCodecCtx,_aSwrInFrame);
    if (ret == AVERROR(EAGAIN) ) {
        return 0;
    }else if(ret == AVERROR_EOF){
//        setState(Stoped);
//        free();
    } else RET(avcodec_receive_frame);


   return resamplePCM();
//   return _aSwrInFrame->nb_samples*_aSwrInSpec.bytesPerSampleFrame;

}

int VideoPlayer::initSDL()
{
    int ret = 0;

    SDL_AudioSpec spec;
    //采样率
    spec.freq = _aSwrOutSpec.sample_rate;
    //采样格式
    spec.format = AUDIO_S16LSB;
    //声道数
    spec.channels = _aSwrOutSpec.chs;
    //音频缓冲区样本数  值为2的n次幂
    spec.samples = 512;
    //回调
    spec.callback = onAudioCallback;

    spec.userdata = this;
    //打开设备
    if(SDL_OpenAudio(&spec,nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        return -1;
    }

    return ret;
}

int VideoPlayer::initSWR()
{
    int ret = 0;
    _aSwrInSpec.chLayout = _aCodecCtx->channel_layout;
    _aSwrInSpec.chs = _aCodecCtx->ch_layout.nb_channels;
    _aSwrInSpec.sample_format = _aCodecCtx->sample_fmt;
    _aSwrInSpec.sample_rate = _aCodecCtx->sample_rate;
    _aSwrInSpec.bytesPerSampleFrame = av_get_bytes_per_sample(_aSwrInSpec.sample_format)
                                       * _aSwrInSpec.chs;


    _aSwrOutSpec.chLayout = AV_CH_LAYOUT_STEREO;
    _aSwrOutSpec.chs = av_get_channel_layout_nb_channels(_aSwrOutSpec.chLayout);
    _aSwrOutSpec.sample_format = AV_SAMPLE_FMT_S16;
    _aSwrOutSpec.sample_rate = 44100;
    _aSwrOutSpec.bytesPerSampleFrame = av_get_bytes_per_sample(_aSwrOutSpec.sample_format)
                                      * _aSwrOutSpec.chs;
    //创建采样上下文
    _SwrCtx = swr_alloc_set_opts(nullptr,
                                         _aSwrOutSpec.chLayout,
                                         _aSwrOutSpec.sample_format,
                                         _aSwrOutSpec.sample_rate,
                                         _aSwrInSpec.chLayout,
                                         _aSwrInSpec.sample_format,
                                         _aSwrInSpec.sample_rate,
                                         0,
                                         nullptr);
    if (!_SwrCtx) {
        qDebug() << "swr_alloc_set_opts error";
        return -1;
    }
    //初始化
    //初始化上下文
    ret = swr_init(_SwrCtx);
    RET(swr_init);

    //初始化缓冲区
    _aSwrInFrame = av_frame_alloc();
    if (!_aSwrInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    _aSwrOutFrame = av_frame_alloc();
    ret = av_samples_alloc(_aSwrOutFrame->data,_aSwrOutFrame->linesize,
                                             _aSwrOutSpec.chs,
                                             4096,
                                             _aSwrOutSpec.sample_format,
                                             1);
    RET(av_samples_alloc);
    return ret;
}

int  VideoPlayer::resamplePCM()
{
    int out_nb_samples = av_rescale_rnd(_aSwrOutSpec.sample_rate,
                                        _aSwrInFrame->nb_samples,
                                        _aSwrInSpec.sample_rate,
                                        AV_ROUND_UP);

    //返回采样数量
    int ret = swr_convert(_SwrCtx,_aSwrOutFrame->data,out_nb_samples,
                      (const uint8_t**)_aSwrInFrame->data,_aSwrInFrame->nb_samples);

    int len = ret * _aSwrOutSpec.bytesPerSampleFrame;
//    if (len > 0) {
//        _file->write((const char *)_aSwrOutFrame->data[0],len);
//    }
    RET(swr_convert);
    return len;

}

void VideoPlayer::onAudioCallback(void *userdata, Uint8 *stream, int len)
{
    VideoPlayer * player = (VideoPlayer*)userdata;
    player->doAudioCallback(stream,len);
}

void VideoPlayer::doAudioCallback(Uint8 *stream, int len)
{

    //清空steam;
    SDL_memset(stream,0,len);
    int volume = _mute ? 0 : SDL_MIX_MAXVOLUME * _volume / 100.0;
    while (len > 0) {
        if (_state == Stoped) {
            break;
        }
        if ( _startOffsetIndex >= _swrOutSize ) {
            _swrOutSize = readAudioPkt();
            if (_swrOutSize <= 0) {
                memset(_aSwrOutFrame->data[0],0,_swrOutSize = 1024);
            }
            _startOffsetIndex = 0;
        }

         int scrLen = _swrOutSize - _startOffsetIndex;
         scrLen = fmin(scrLen,len);

         SDL_MixAudio(stream,_aSwrOutFrame->data[0] + _startOffsetIndex,scrLen,volume);
         len -= scrLen;
         stream += scrLen;
         _startOffsetIndex += scrLen;
    }
}


void VideoPlayer::freeAudio() {

    clearAudioPktList();
    avcodec_free_context(&_aCodecCtx);
    swr_free(&_SwrCtx);
    av_frame_free(&_aSwrInFrame);
    if (_aSwrOutFrame) {
        av_freep(&_aSwrOutFrame->data[0]);
        av_frame_free(&_aSwrOutFrame);
    }
    _startOffsetIndex = 0;
    _swrOutSize = 0;
    _aClock = 0;
    // 停止播放
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}
