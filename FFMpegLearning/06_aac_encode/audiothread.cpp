#include "audiothread.h"
#include  <QDebug>
#include <QFile>
#include <QTime>
#include "CommonDefine.h"

#include "ffmpegs.h"
extern "C" {
// 设备相关API
#include <libavdevice/avdevice.h>
// 格式相关API
#include <libavformat/avformat.h>
// 工具相关API（比如错误处理）
#include <libavutil/avutil.h>
// 编码相关API
#include <libavcodec/avcodec.h>
}


// 从AVFormatContext中获取录音设备的相关参数
void showSpec(AVFormatContext *ctx) {
    // 获取输入流
    AVStream *stream = ctx->streams[0];
    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;
    // 声道数
    qDebug() << "channels"<<params->ch_layout.nb_channels;
    // 采样率
    qDebug() <<"sample_rate"<< params->sample_rate;
    // 采样格式
    qDebug() <<"format"<< params->format;
    // 每一个样本的一个声道占用多少个字节
    qDebug() << "av_get_bytes_per_sample"<<av_get_bytes_per_sample((AVSampleFormat) params->format);
    // 编码ID（可以看出采样格式）
    qDebug() << "codec_id"<<params->codec_id;
    // 每一个样本的一个声道占用多少位（这个函数需要用到avcodec库）
    qDebug() <<"av_get_bits_per_sample"<<av_get_bits_per_sample(params->codec_id);


}

AudioThread::AudioThread(QObject *parent):QThread(parent)
{
    connect(this,&AudioThread::finished,this,&AudioThread::deleteLater);

}

AudioThread::~AudioThread()
{
    disconnect ();
    requestInterruption();
    quit();
    wait();

    qDebug() << "销毁AudioThread";
}

void AudioThread::run()
{

#ifdef Q_OS_WIN
EncodeAudioSpec spec;
spec.chLayout = AV_CHANNEL_LAYOUT_STEREO ;
spec.sample_rate = 44100;
spec.filename = OUTPCMFILENAME;
spec.sample_format = AV_SAMPLE_FMT_S16;
#else
    EncodeAudioSpec spec;
    spec.chLayout = AV_CHANNEL_LAYOUT_STEREO ;
    spec.sample_rate = 44100;
    spec.filename = OUTPCMFILENAME;
    spec.sample_format = AV_SAMPLE_FMT_S16;

#endif
const char * outFIle = OUTAACFILENAME;
EncodeAudioOutSpec outSpec;
outSpec.outFile = outFIle;
FFmpegs::aacEncode(spec,outSpec);
}


//播放录制的PCM数据
/* Mac
 *使用自带麦克风录制数据播放命令:
 *ffplay -sample_rate 48000 -channels 1 -f f32le  out.pcm
 *
 *Windows
 *ffplay -ar 44100 -ac 2 -f s16le out.pcm
 */
