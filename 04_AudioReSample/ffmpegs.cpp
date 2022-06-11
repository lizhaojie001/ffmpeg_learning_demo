#include "ffmpegs.h"
#include <QFile>
#include <QDebug>
FFmpegs::FFmpegs(QObject *parent)
    : QObject{parent}
{

}

void FFmpegs::pcm2wav(WAVHeader &header, const char *pcmFilename, const char *wavFilename)
{
    header.BlockAlign = header.NumChannels * header.BitsPerSample >> 3;
    header.ByteRate = header.SampleRate * header.BlockAlign >> 3;
    //打开文件pcm
    QFile pcmFile(pcmFilename);
    if (!pcmFile.open(QFile::ReadOnly)) {
        qDebug() << "文件打开失败" << pcmFilename;
        return;
    }
    header.DatachunkSize = pcmFile.size();
    header.ChunkSize = header.DatachunkSize
            + sizeof (WAVHeader) - 8;
    //打开wav
    QFile wavFile(wavFilename);
    if (!wavFile.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << wavFilename;
        pcmFile.close();
        return;
    }

    //写入头文件
    wavFile.write((const char *)&header, sizeof(WAVHeader));
    //写入pcm数据
    char buffer[1024];
    uint64_t len;
    while ((len = pcmFile.read(buffer,sizeof(buffer))) > 0) {
             wavFile.write(buffer,len);
    }


    pcmFile.close();
    wavFile.close();


}

void FFmpegs::resampleAudio(ResampleAudioSpec in, ResampleAudioSpec out)
{
    QString outFilename = QString::fromStdString(out.filename);
    QFile out_file(outFilename);
    QString intFilename = QString::fromStdString(in.filename);;
    QFile in_file(intFilename);
#ifdef Q_OS_WIN

    uint8_t ** out_audio_data = nullptr;
    int out_linesize = 0;
    int out_nb_channels = av_get_channel_layout_nb_channels(out.chLayout);

    int out_align = 1;

    int out_bytes_per_sample = av_get_bytes_per_sample(out.sample_format) * out_nb_channels;
    uint8_t ** in_audio_data = nullptr;
    int in_linesize = 0;
    int in_nb_channels = av_get_channel_layout_nb_channels(in.chLayout);
    int in_nb_samples = 1024;
    int in_align = 1;

#else

     //创建上下文


    uint8_t ** out_audio_data = nullptr;
    int out_linesize = 0;
    int out_nb_channels = av_get_channel_layout_nb_channels(out.chLayout);

    int out_align = 1;

    int out_bytes_per_sample = av_get_bytes_per_sample(out.sample_format) * out_nb_channels;
    uint8_t ** in_audio_data = nullptr;
    int in_linesize = 0;
    int in_nb_channels = av_get_channel_layout_nb_channels(in.chLayout);
    int in_nb_samples = 1024;
    int in_align = 1;
#endif
     int out_nb_samples = av_rescale_rnd(out.sample_rate,in_nb_samples,in.sample_rate,AV_ROUND_UP);
     //out_sample_rate * in_nb_samples / in_sample_rate;
    SwrContext  *cxt = swr_alloc_set_opts(nullptr,
                                          out.chLayout,
                                          out.sample_format,
                                          out.sample_rate,
                                          in.chLayout,
                                          in.sample_format,
                                          in.sample_rate,
                                          0,
                                          nullptr);

     int ret = 0;
     int len = 0;

    if(!cxt) {
        qDebug() << "swr_alloc_set_opts error";
        goto end;

    }
    //初始化上下文
   ret = swr_init(cxt);

   if(ret < 0) {
       ERROR(ret)
       qDebug() << "swr_init error " << errbuf ;
      goto end;
   }

   //创建输出缓冲区

   ret = av_samples_alloc_array_and_samples(&out_audio_data,&out_linesize,out_nb_channels,out_nb_samples,out.sample_format,out_align);
    if(ret < 0) {
        ERROR(ret);
        qDebug() << "av_samples_alloc_array_and_samples" << errbuf;
        goto end;
    }

    //创建输入缓冲区

    ret = av_samples_alloc_array_and_samples(&in_audio_data,&in_linesize,in_nb_channels,in_nb_samples,in.sample_format,in_align);
     if(ret < 0) {
         ERROR(ret);
         qDebug() << "av_samples_alloc_array_and_samples" << errbuf;
         goto end;
     }


     //打开文件
     if(!out_file.open(QFile::WriteOnly)) {
         qDebug() << "out_file open error";
         goto end;
     }

     if(!in_file.open(QFile::ReadOnly)) {
         qDebug() << "out_file open error";
         goto end;
     }

     while ((len = in_file.read((char *)in_audio_data[0],in_linesize)) > 0) {
         //实际读取的长度
         int in_count = len / (av_get_bytes_per_sample(in.sample_format) * in_nb_channels);
         //返回采样数量
         ret = swr_convert(cxt,out_audio_data,out_nb_samples,
                           (const uint8_t**)in_audio_data,in_count);
         if(ret < 0) {
             ERROR(ret)
                     qDebug()<< "swr_convert error" << errbuf;
             goto end;
         }

         out_file.write((char *)out_audio_data[0],ret * out_bytes_per_sample);
     }

// 处理缓冲区残留样本
     while ((ret = swr_convert(cxt,out_audio_data,out_nb_samples,
                           nullptr,0)) > 0) {
         out_file.write((char *)out_audio_data[0],ret * out_bytes_per_sample);

     }

 end:
     //资源释放
     out_file.close();
     in_file.close();
     if(out_audio_data){
      av_freep(&out_audio_data[0]);
     }
     av_freep(&out_audio_data);
     if (in_audio_data) {
             av_freep(&in_audio_data[0]);
     }
    av_freep(&in_audio_data);
    swr_free(&cxt);

}

