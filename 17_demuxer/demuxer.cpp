#include "demuxer.h"
#include <QDebug>
extern "C"{
#include <libavutil/imgutils.h>
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
Demuxer::Demuxer(QObject *parent)
    : QObject{parent}
{

}

void Demuxer::demuxer(const char *inFilename, DecodeAudioSpec &aOut, DecodeVideoSpec &vOut)
{

    _aOut = &aOut;
    _vOut = &vOut;
    int ret = 0;
    int index = 0;
    AVPacket *pkt = nullptr;

    QFile inFile(inFilename);
    if(!inFile.open(QFile::ReadOnly)){
        qDebug() << "open file error" << inFilename;
        return ;
    }

    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, inFilename, nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 打印流信息到控制台
    av_dump_format(_fmtCtx, 0, inFilename, 0);
    fflush(stderr);

    //初始化视频模块
    ret = initVideoInfo();
    END(initVideoInfo);

    //初始化音频模块
    ret = initAudioInfo();
    END(initAudioInfo);


    //创建缓存区
    _frame = av_frame_alloc();
    if (!_frame) {
        END(av_frame_alloc);
    }

    pkt = av_packet_alloc();
    pkt->data = nullptr;
    pkt->size = 0;

    //读取数据
    while (av_read_frame(_fmtCtx,pkt) == 0) {
        if(pkt->stream_index == _aStreamIndex) {
            qDebug() << pkt->data << pkt->size << index++;
          ret  = decode(_aCodecCtx,pkt,&Demuxer::writeAudio);
        } else if(pkt->stream_index == _vStreamIndex) {
            ret = decode(_vCodecCtx,pkt,&Demuxer::writeVideo);
        }

        av_packet_unref(pkt);
        END(decode);
    }
     decode(_aCodecCtx,nullptr,&Demuxer::writeAudio);
     decode(_vCodecCtx,nullptr,&Demuxer::writeVideo);
end:
    _aFile.close();
    _vFile.close();
    av_frame_free(&_frame);
    av_packet_free(&pkt);
    avcodec_close(_aCodecCtx);
    avcodec_close(_vCodecCtx);
    avformat_close_input(&_fmtCtx);
    av_freep(&_data);

}

int Demuxer::initVideoInfo() {

    int ret = 0;
    ret = initDecoder(&_vCodecCtx,AVMEDIA_TYPE_VIDEO,&_vStreamIndex);

    //打开文件
//    QFile inF(_vOut->filename);
    _vFile.setFileName(_vOut->filename);
    if (!_vFile.open(QFile::WriteOnly)) {
        qDebug() << "open file error" << _vOut->filename;
        return -1;
    }


    //设置参数
    _vOut->height = _vCodecCtx->height;
    _vOut->width = _vCodecCtx->width;
    _vOut->pix_fmt = _vCodecCtx->pix_fmt;

    //一帧大小
    _imageSize = av_image_get_buffer_size(_vOut->pix_fmt,_vOut->width,_vOut->height,1);


    //初始化一帧数据
    ret = av_image_alloc(_data,_lineSize,_vOut->width,_vOut->height,_vOut->pix_fmt,1);

    return ret;
}

int Demuxer::initAudioInfo() {
    initDecoder(&_aCodecCtx,AVMEDIA_TYPE_AUDIO,&_aStreamIndex);


    //打开文件
//    QFile inF(_aOut->filename);
    _aFile.setFileName(_aOut->filename);
    if (!_aFile.open(QFile::WriteOnly)) {
        qDebug() << "open file error" << _aOut->filename;
        return -1;
    }

    //设置参数
    _aOut->sample_fmt = _aCodecCtx->sample_fmt;
    _aOut->sample_rate = _aCodecCtx->sample_rate;
    _aOut->channels = _aCodecCtx->ch_layout.nb_channels;

    //计算单声道采样数据大小
    _per_sample_size = av_get_bytes_per_sample(_aCodecCtx->sample_fmt);
    _per_sample_frame_size = _per_sample_size * _aOut->channels;
    //是否是planar
    _is_planar = av_sample_fmt_is_planar(_aCodecCtx->sample_fmt);
    qDebug() << "_is_planar:  " << _is_planar;

    return 0;
}

int Demuxer::initDecoder(AVCodecContext**ctx,AVMediaType type, int * streamIndex)
{
    //从流信息中获取数据
    int ret = av_find_best_stream(_fmtCtx,type,-1,-1,nullptr,0);
    RET(ret);
    *streamIndex = ret;
    //获取stream
    AVStream * st = _fmtCtx->streams[ret];
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


    return 0;


}



int Demuxer::decode(AVCodecContext * ctx, AVPacket *pkt, void (Demuxer::*func)())
{
    //给解码器发数据解码
    int ret = avcodec_send_packet(ctx,pkt);
    RET(avcodec_send_packet);

    while (true) {
        ret = avcodec_receive_frame(ctx,_frame);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return   0;
        }  else if(ret < 0){
            return ret;
        }
        (this->*func)();
    }
}

void Demuxer::writeAudio()
{
    //可以播放的pcm数据都是非planer类型的,planer是ffmpeg中自制的格式,不是通用格式,是为了方便处理数据生成的
    /*
    LLLLLL   RRRRRR   DDDDDD     ===>   LRD   LRD  LRD  LRD  LRD
    |--0--|  |--1--|  |--2--|

    */
      // is planer
    if( _is_planar) {
//        for (int l = 0; l < _frame->nb_samples; ++l) {
//            for (int s = 0; s < _aOut->channels; ++s) {
//                char *begin = (char *) (_frame->data[s] + l * _per_sample_size);
//                _aFile.write(begin, _per_sample_size);
//            }
//        }
        for (int si = 0; si < _frame->nb_samples; si++) {
            // 内层循环：有多少个声道
            // ci = channel index
            for (int ci = 0; ci < _aOut->channels; ci++) {
                char *begin = (char *) (_frame->data[ci] + si * _per_sample_size);
                _aFile.write(begin, _per_sample_size);
            }
        }
    } else {
        _aFile.write((char*)_frame->data[0],_frame->nb_samples*_per_sample_frame_size );
    }
}

void Demuxer::writeVideo()
{
    //创建av_image 自动填充数据
    av_image_copy(_data,_lineSize,(const uint8_t **)_frame->data,_frame->linesize,_vOut->pix_fmt,_vOut->width,_vOut->height);
    _vFile.write((char *)_data[0],_imageSize);
}
