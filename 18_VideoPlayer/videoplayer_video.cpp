#include "videoplayer.h"
#include <QDebug>
#include <thread>
extern "C"{
#include <libavutil/imgutils.h>
}


int VideoPlayer::initVideoInfo()
{
    int ret = 0;
    ret = initDecoder(&_vCodecCtx,AVMEDIA_TYPE_VIDEO,&_vStream);

    RET(initDecoder);

    ret = initSws();
    RET(initSws);


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
    while (true) {
        qDebug() << "读取视频数据";
        if (_state == Stoped) {
            break;
        }
        //解码数据
        //给解码器发数据解码
            _vMutex->lock();
            if (_vPktList->empty()) {
                _vMutex->unlock();
                SDL_Delay(33);
                continue ;
            }

            AVPacket pkt = _vPktList->front();
            _vPktList->pop_front();
            _vMutex->unlock();
            int ret = avcodec_send_packet(_vCodecCtx,&pkt);
            av_packet_unref(&pkt);
            if (ret < 0) {
                qDebug() << "avcodec_send_packet";
                break;
            }
            while (true) {
                ret = avcodec_receive_frame(_vCodecCtx,_vSwsInFrame);
                if (ret == AVERROR(EAGAIN) ) {
                    qDebug() << "avcodec_receive_frame";
                    break;
                }else if(ret == AVERROR_EOF){
                    qDebug() << "avcodec_receive_frame eof";
                    break;
                } else {
                    qDebug() << "avcodec_receive_frame error";
                    break;
                }
            }
            //格式转换
            ret = sws_scale(_vSwsCtx,_vSwsInFrame->data,_vSwsInFrame->linesize,
                            0,_vCodecCtx->height,
                            _vSwsOutFrame->data,_vSwsOutFrame->linesize);

            SDL_Delay(33);
            //发送信号数据
            emit playerVideoDeceoded(this,_vSwsOutFrame->data[0],_vSwsOutSpec);


    }
}

void VideoPlayer::freeVideo(){

}

int VideoPlayer::initSws()
{
    _vSwsOutSpec.width = _vCodecCtx->width >> 4 << 4;
    _vSwsOutSpec.height = _vCodecCtx->height >> 4 << 4;
    _vSwsOutSpec.pix_fmt = AV_PIX_FMT_RGB24;
    _vSwsCtx = sws_getContext(_vCodecCtx->width,_vCodecCtx->height,_vCodecCtx->pix_fmt,
                              _vSwsOutSpec.width,_vSwsOutSpec.height ,_vSwsOutSpec.pix_fmt,
                              SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!_vSwsCtx) {
        qDebug() << "sws_getContext error";
        return -1;
    }

    //初始化像素输入缓存区
    _vSwsInFrame = av_frame_alloc();
    if(!_vSwsInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }
    //初始化像素输出缓存区
    _vSwsOutFrame = av_frame_alloc();
    if(!_vSwsOutFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }
   int ret = av_image_alloc(_vSwsOutFrame->data,_vSwsOutFrame->linesize,
                         _vSwsOutSpec.width,_vSwsOutSpec.height,_vSwsOutSpec.pix_fmt,
                         1);
    RET(av_image_alloc);

   return 0;
}
