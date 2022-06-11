#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
FFmpegs::FFmpegs(QObject *parent)
    : QObject{parent}
{

}

void FFmpegs::decodeH264(const char *inFilename, DecodeVideoSpec &outSpec)
{

        int ret = 0;
        bool isEnd = false;
        int inLen = 0;
        //编解码上下文
       AVCodecContext *ctx =  nullptr;
       //解码器上下文
       AVCodecParserContext * parserCtx = nullptr;

       char dataArray[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
       char * inData = dataArray;
       //解码数据pkt
       AVPacket *   pkt = nullptr;
       //接收数据
       AVFrame * frame = nullptr;
       // 获取解码器
       const AVCodec * decodec = avcodec_find_decoder(AV_CODEC_ID_H264);
       if(!decodec) {
           qDebug() << "avcodec_find_decoder_by_name error" ;
           return;
       }
        //        输入文件
        QFile inFile(inFilename);
        QFile outFile(outSpec.filename);
        if(!inFile.open(QFile::ReadOnly))
        {
            qDebug() << " inFile　error" ;
            goto end;
        }

        if(!outFile.open(QFile::WriteOnly))
        {
            qDebug() << " outFile　error" ;
            goto end;
        }
       //创建上下文
       ctx = avcodec_alloc_context3(decodec);
       if(!ctx){
          qDebug() << "avcodec_alloc_context3 error";
          goto end;
       }

       //创建解码器上下文
       parserCtx = av_parser_init(decodec->id);

       if(!parserCtx) {
           qDebug() << "av_parser_init error";
           goto end;
       }

       pkt = av_packet_alloc();
       if(!pkt) {
           qDebug() << "av_packet_alloc error";
           goto end;
       }

       frame = av_frame_alloc();
       if(!frame) {
           qDebug() << "av_frame_alloc error";
           goto end;
       }

       //打开解码器
       ret = avcodec_open2(ctx,decodec,nullptr);
       if(ret < 0) {
           ERROR(ret);
           qDebug() << "avcodec_open2 error" << errbuf;
           goto end;
       }

    // 读取数据


       do{
           inLen = inFile.read(dataArray, AUDIO_INBUF_SIZE);
           isEnd = !inLen;
           inData = dataArray;
            while (inLen > 0 || isEnd) {
               // 经过解析器上下文处理
               ret = av_parser_parse2(parserCtx, ctx,
                                      &pkt->data, &pkt->size,
                                      (uint8_t *) inData, inLen,
                                      AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
               if (ret < 0) {
                   ERROR(ret);
                   qDebug() << "av_parser_parse2 error" << errbuf;
                   goto end;
               }
               // 跳过已经解析过的数据
               inData += ret;
               // 减去已经解析过的数据大小
               inLen -= ret;
               // 解码
               if (pkt->size > 0 && decode(ctx, frame, pkt, outFile) < 0)
               {
                   goto end;
               }
               if (isEnd) {
                   break;
               }
            }


       }while (!isEnd);

       // flush解码器
       //pkt->data = NULL;
       //pkt->size = 0;
       decode(ctx, frame, nullptr , outFile);

        outSpec.width = ctx->width;
        outSpec.height = ctx->height;
        outSpec.pix_fmt = ctx->pix_fmt;
        outSpec.fps = ctx->framerate.num;

end:
       inFile.close();
       outFile.close();
       av_packet_free(&pkt);
       av_frame_free(&frame);
//关闭解码器
       av_parser_close(parserCtx);
       avcodec_free_context(&ctx);
}
//0 success  负数失败
int FFmpegs::decode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, QFile &outFile)
{

    //给解码器发数据解码
    int ret = avcodec_send_packet(ctx,pkt);
    if (ret < 0) {
        ERROR(ret);
        qDebug() << "avcodec_send_frame error" << errbuf;
        return  ret;
    }

    while (true) {
        ret = avcodec_receive_frame(ctx,frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return  0;
        }  else if(ret < 0){
            return ret;
        }
        outFile.write((const char*)frame->data[0],frame->linesize[0] * ctx->height);
        outFile.write((const char*)frame->data[1],frame->linesize[1] * ctx->height >>1);
        outFile.write((const char*)frame->data[2],frame->linesize[2] * ctx->height >>1);


    }
}


