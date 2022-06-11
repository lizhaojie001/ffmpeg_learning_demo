#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

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


void FFmpegs::aacEncode(EncodeAudioSpec &spec, EncodeAudioOutSpec &outSpec)
{

    int ret = 0;
    QString infileName = QString::fromStdString(spec.filename);
    QFile inFile(infileName);
    QFile outFile(QString::fromStdString(outSpec.outFile));

    //    编码器
     const   AVCodec * codec = avcodec_find_encoder_by_name("libfdk_aac");
    //编码器上下文
     AVCodecContext * ctx = avcodec_alloc_context3(codec);

     //PCM缓存数据
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
    ctx->sample_fmt = spec.sample_format;
    ctx->sample_rate = spec.sample_rate;
    ctx->ch_layout = spec.chLayout;

    //打开设备
    ret = avcodec_open2(ctx,codec,nullptr);
    if(ret < 0) {
        ERROR(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }

    frame->sample_rate = ctx->sample_rate;
    frame->nb_samples = ctx->frame_size;
    frame->ch_layout = ctx->ch_layout;
    frame->format = ctx->sample_fmt;
    // 创建AVFrame内部的缓冲区
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        ERROR(ret);
        qDebug() << "av_frame_get_buffer error" << errbuf;
        goto end;
    }
    while ( (ret = inFile.read(( char *)frame->data[0],frame->linesize[0])) >0) {
            if(ret < frame->linesize[0]) {
                //最后一次填充数据没有填满
//                重新设置一下样本数量
                int bytePerSample = av_get_bytes_per_sample(ctx->sample_fmt);
                int ch_num = ctx->ch_layout.nb_channels;
                frame->nb_samples = ret / (bytePerSample * ch_num);
            }
           if (encode(ctx,frame,pkt,outFile) != 0) {
               goto end;
           }
    }
    //刷缓存数据
    encode(ctx,nullptr,pkt,outFile);
    av_packet_unref(pkt);
end:
    inFile.close();
    outFile.close();
    av_packet_free(&pkt);
    av_frame_free(&frame);
    avcodec_free_context(&ctx);

}


