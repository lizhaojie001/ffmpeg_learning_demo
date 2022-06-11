#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

extern "C" {
#include<libavutil/imgutils.h>
}
FFmpegs::FFmpegs(QObject *parent)
    : QObject{parent}
{

}

// return
// 0 表示转码成功
// 负数表示失败
int FFmpegs::encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, QFile &outFile)
{
    //给编码器发数据让他编码
    int ret = avcodec_send_frame(ctx,frame);
    if (ret < 0) {
        ERROR(ret);
        qDebug() << "avcodec_send_frame error" << errbuf;
        return  ret;
    }

    while (true) {
        ret = avcodec_receive_packet(ctx,pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return  0;
        }  else if(ret < 0){
            return ret;
        }
          outFile.write((char*)pkt->data,pkt->size);
          av_packet_unref(pkt);
    }
}


void FFmpegs::yuvEncode(YUVEncodeSpec &spec, const char *outFilename)
{

    int ret = 0;
    QFile inFile(spec.filename);
    QFile outFile(outFilename);

    int imageSize = av_image_get_buffer_size(spec.fmt,spec.w,spec.h,1);
    //    编码器
     const   AVCodec * codec = avcodec_find_encoder_by_name("libx264");
    //编码器上下文
     AVCodecContext * ctx = avcodec_alloc_context3(codec);

     //yuv缓存数据
     AVFrame *frame = av_frame_alloc();
     //转码后数据
     AVPacket * pkt = av_packet_alloc();

     if (!frame) {
         goto end;
     }

     if (!pkt) {
         goto  end;
     }

     if (!codec) {
         qDebug() << "avcodec_find_encoder_by_name error";
         goto end;
     }

     if (!ctx) {
         qDebug() << "avcodec_alloc_context3 error";
         goto end;
     }

    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "inFile open error";
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "outFile open error";
        goto end;
    }
    ctx->width = spec.w;
    ctx->height = spec.h;
    ctx->time_base = {1 , spec.fps};
    ctx->pix_fmt = spec.fmt;
    //打开设备
    ret = avcodec_open2(ctx,codec,nullptr);
    if(ret < 0) {
        ERROR(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }

    frame->width = ctx->width;
    frame->height = ctx->height;
    frame->format = spec.fmt;
    frame->pts = 0;
        // 创建AVFrame内部的缓冲区
//    ret = av_frame_get_buffer(frame, 0);
    ret = av_image_alloc(frame->data,frame->linesize,spec.w,spec.h,spec.fmt,1);
    if (ret < 0) {
        ERROR(ret);
        qDebug() << "av_frame_get_buffer error" << errbuf;
        goto end;
    }
    while ( (ret = inFile.read(( char *)frame->data[0],imageSize)) >0) {
           if (encode(ctx,frame,pkt,outFile) != 0) {
               goto end;
           }
           frame->pts++;
    }
    encode(ctx,nullptr,pkt,outFile);
end:

    inFile.close();
    outFile.close();
    av_packet_free(&pkt);
    if (frame) {
        av_freep(&frame_data[0]);
    }
    av_frame_free(&frame);
    avcodec_free_context(&ctx);

}


