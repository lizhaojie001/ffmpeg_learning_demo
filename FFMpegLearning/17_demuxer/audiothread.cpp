#include "audiothread.h"
#include  <QDebug>
#include <QFile>
#include <QTime>
//#include "CommonDefine.h"
#include "demuxer.h"
extern "C" {

// 格式相关API
#include <libavformat/avformat.h>
// 工具相关API（比如错误处理）
#include <libavutil/avutil.h>
// 编码相关API
#include <libavcodec/avcodec.h>

#include <libavutil/imgutils.h>
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

DecodeVideoSpec vOut;
vOut.filename = "E:/out_demuxer.yuv";

DecodeAudioSpec aOut;
aOut.filename =  "E:/out_demuxer.pcm";


char * inFile = "E:/outBD1280.mp4";

Demuxer().demuxer(inFile,aOut,vOut);
qDebug() <<  vOut.width << vOut.height << av_get_pix_fmt_name(vOut.pix_fmt) << vOut.fps;
qDebug() << aOut.sample_rate << aOut.channels << av_get_sample_fmt_name(aOut.sample_fmt);

}


//播放录制的PCM数据
/* Mac
 *使用自带麦克风录制数据播放命令:
 *ffplay -sample_rate 48000 -channels 1 -f f32le  out.pcm
 *
 *Windows
 *ffplay -ar 44100 -ac 2 -f s16le out.pcm
 */
