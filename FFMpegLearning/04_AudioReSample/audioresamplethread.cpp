#include "audioresamplethread.h"
#include <QDebug>
#include "ffmpegs.h"
extern "C" {
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}
AudioReSampleThread::AudioReSampleThread(QObject *parent)
    : QThread{parent}
{
    connect(this,&AudioReSampleThread::finished,this,&AudioReSampleThread::deleteLater);
}

AudioReSampleThread::~AudioReSampleThread()
{
    requestInterruption();
    wait();
    quit();
    qDebug() << "AudioReSampleThread" << "析构";
}


void AudioReSampleThread::run()
{

#ifdef Q_OS_WIN
    std::string prefFile = "E:/";
    std::string inFilename =  "E:/out.pcm";
    int in_ch_layout = AV_CH_LAYOUT_STEREO;
    AVSampleFormat in_sample_format = AV_SAMPLE_FMT_S16;
    int in_sample_rate = 44100;
    ResampleAudioSpec in = {inFilename,in_ch_layout,in_sample_rate,in_sample_format };
#else
    std::string prefFile = "/Users/macbookpro/Downloads/";
    std::string inFilename =  prefFile + "out.pcm";
    int in_ch_layout = AV_CH_LAYOUT_MONO;
    AVSampleFormat in_sample_format = AV_SAMPLE_FMT_FLT;
    int in_sample_rate = 48000;
    ResampleAudioSpec in = {inFilename,in_ch_layout,in_sample_rate,in_sample_format };
#endif


    std::string outFilename = prefFile + "out_1_f32le_48000.pcm";
    int out_ch_layout = AV_CH_LAYOUT_MONO;
    AVSampleFormat out_sample_format  = AV_SAMPLE_FMT_FLT;
    int out_sample_rate = 48000;

    ResampleAudioSpec out = {outFilename,out_ch_layout,out_sample_rate,out_sample_format };

    FFmpegs::resampleAudio(in,out);



    std::string out1Filename = prefFile + "out_1_s32le_48000.pcm";
    int out1_ch_layout = AV_CH_LAYOUT_MONO;
    AVSampleFormat out1_sample_format  = AV_SAMPLE_FMT_S32;
    int out1_sample_rate = 48000;
    ResampleAudioSpec out1= {out1Filename,out1_ch_layout,out1_sample_rate,out1_sample_format };
    FFmpegs::resampleAudio(out,out1);

    std::string in1Filename = prefFile + "out_2_s16le_44100.pcm";;
    int in1_ch_layout = AV_CH_LAYOUT_STEREO;
    AVSampleFormat in1_sample_format = AV_SAMPLE_FMT_S16;
    int in1_sample_rate = 44100;
    ResampleAudioSpec in1 = {in1Filename,in1_ch_layout,in1_sample_rate,in1_sample_format };
    FFmpegs::resampleAudio(out1,in1);

}
