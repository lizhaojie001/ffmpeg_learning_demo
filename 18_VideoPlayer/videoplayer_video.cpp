#include "videoplayer.h"
extern "C"{
#include <libavutil/imgutils.h>
}


int VideoPlayer::initVideoInfo()
{
    int ret = 0;
    ret = initDecoder(&_vCodecCtx,AVMEDIA_TYPE_VIDEO,&_vStream);

    //一帧大小
    _imageSize = av_image_get_buffer_size(_vCodecCtx->pix_fmt,_vCodecCtx->width,_vCodecCtx->height,1);

    return ret;
}



void VideoPlayer::clearVideoPktList()
{
    _vMutex->lock();
    while (_vPktList->size() > 0) {
        AVPacket pkt = _vPktList->front();
        av_packet_unref(&pkt);
        _vPktList->pop_front();
    }
    _vMutex->unlock();

}

void VideoPlayer::addVideoPkt(AVPacket &pkt)
{
    _vMutex->lock();
    _vPktList->push_back(pkt);
    _vMutex->signal();
    _vMutex->unlock();
}

void VideoPlayer::readVideoPkt()
{

}
