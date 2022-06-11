#include "ffmpegs.h"
#include <QDebug>
extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
}

#include <QFile>
#define RET(judge, func) \
if (!judge) { \
        qDebug() << #func << "Error" ; \
        return;\
}

#define END(func) \
if (!ret) { \
        qDebug() << #func << "Error" ; \
        goto end; \
}


FFmpegs::FFmpegs(QObject *parent)
    : QObject{parent}
{

}

void FFmpegs::transformFile(RawVideoFile &in, RawVideoFile &out)
{
    //创建转换上下文

    SwsContext *ctx = nullptr;

    //输入一帧大小

    int inFrameSize = 0;

    //输出一帧大小
    int outFrameSize = 0;

    int ret = 0;

    uint8_t *srcSlice[4] , *dst[4];
    int srcStride[4];
    int dstStride[4];


    //创建上下文
    ctx = sws_getContext(in.w,in.h,in.fmt,out.w,out.h,out.fmt,SWS_BILINEAR,nullptr,nullptr,nullptr);
    RET(ctx,sws_alloc_context);

    //打开文件
    QFile infile(in.filename);
    QFile outfile(out.filename);

    ret = infile.open(QFile::ReadOnly);
    END(inFileopen);

    ret = outfile.open(QFile::WriteOnly);
    END(outFileopen);

    //转换图片

    inFrameSize = av_image_get_buffer_size(in.fmt,in.w,in.h,1);

    outFrameSize = av_image_get_buffer_size(out.fmt,out.w,out.h,1);

    //初始化内存
    ret = av_image_alloc(&srcSlice[0],srcStride,in.w,in.h,in.fmt,1);
    END(in_av_image_alloc);
    ret = av_image_alloc(&dst[0],dstStride,out.w,out.h,out.fmt,1);
    END(out_av_image_alloc);

    while (infile.read((char*)srcSlice[0],inFrameSize) == inFrameSize) {
        sws_scale(ctx,&srcSlice[0],srcStride,0,in.h,&dst[0],dstStride);
        outfile.write((char*)dst[0],outFrameSize);
    }
end:
    infile.close();
    outfile.close();
    av_freep(&srcSlice[0]);
    av_freep(&dst[0]);
    sws_freeContext(ctx);

}

void FFmpegs::transformFile(RawVideoFrame &in, RawVideoFrame &out)
{
    //创建转换上下文

    SwsContext *ctx = nullptr;

    //输入一帧大小

    int inFrameSize = 0;

    //输出一帧大小
    int outFrameSize = 0;

    int ret = 0;

    uint8_t *srcSlice[4] , *dst[4];
    int srcStride[4];
    int dstStride[4];


    //创建上下文
    ctx = sws_getContext(in.w,in.h,in.fmt,out.w,out.h,out.fmt,SWS_BILINEAR,nullptr,nullptr,nullptr);
    RET(ctx,sws_alloc_context);


    //转换图片

    inFrameSize = av_image_get_buffer_size(in.fmt,in.w,in.h,1);

    outFrameSize = av_image_get_buffer_size(out.fmt,out.w,out.h,1);

    //初始化内存
    ret = av_image_alloc(&srcSlice[0],srcStride,in.w,in.h,in.fmt,1);
    END(in_av_image_alloc);
    memcpy(srcSlice[0],in.pixels,inFrameSize);

    ret = av_image_alloc(&dst[0],dstStride,out.w,out.h,out.fmt,1);
    END(out_av_image_alloc);

    ret = sws_scale(ctx,&srcSlice[0],srcStride,0,in.h,&dst[0],dstStride);
    END(sws_scale);
    out.pixels = malloc((size_t)outFrameSize);
    memcpy(out.pixels,dst[0],outFrameSize);

end:
    av_freep(&srcSlice[0]);
    av_freep(&dst[0]);
    sws_freeContext(ctx);
}
