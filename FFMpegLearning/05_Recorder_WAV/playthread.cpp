#include "playthread.h"
#include <QtDebug>
#include <QFile>
#include "CommonDefine.h"
extern "C" {
    #include <SDL2/SDL.h>
}



//全局变量默认会初始化
//int buffer_length;
//char * buffer_data;

typedef struct {
    int len ;
    uint8_t * data ;
    int pullLen ;
} AudioBuffer;

//子线程
//音频设备会多次回调
void pull_data_AudioCallback(void *userdata,
                           //需要填充的数据PCM
                           Uint8 * stream,
                           //希望填充的样本大小
                           int len)
{
    AudioBuffer * buffer = (AudioBuffer*)userdata;
  //希望填充的样本大小
  //是根据设置的 (simples 1024) 样本数  x 每个样本的大小(16位 x 2声道数 = 32bit /8 = 4byte) = 4096byte = 4.096kb

    //清空steam;
    SDL_memset(stream,0,len);

    if (buffer->len <= 0) {
        return;
    }

    //填充数据
    SDL_MixAudio(stream,(Uint8 *)buffer->data,len,SDL_MIX_MAXVOLUME);
    //
    buffer->pullLen = len;
    buffer->data +=len;
    buffer->len -= len;
}

/*
*SDL播放音频有两种模式
* 1. push [程序]主动推送音频数据[音频设备]
* 2. pull [音频设备]音频设备主动向[程序]拉取数据
*
*/

PlayThread::PlayThread(QObject *parent): QThread(parent)
{
    connect(this,&PlayThread::finished,this,&PlayThread::deleteLater);
}

PlayThread::~PlayThread()
{
    disconnect();
    requestInterruption();
    wait();
    quit();
    qDebug() << "~PlayThread";
}

void PlayThread::run()
{
    //初始化子系统
    int ret = SDL_Init(SDL_INIT_AUDIO);
    if (ret != 0) {
        const char * err = SDL_GetError();
        qDebug() << err ;
        SDL_Quit();
        return;

    }
    SDL_AudioSpec spec;
    Uint8 * data;
    Uint32 len = 0;


    if (!SDL_LoadWAV(OUTWAVFILENAME, &spec, &data, &len)){
            qDebug() << "SDL_LoadWAV error" << SDL_GetError();
            SDL_Quit();
            return;
    }

    //回调
    spec.callback = pull_data_AudioCallback;

    AudioBuffer buffer;
    buffer.len = len;
    buffer.data = data;
    spec.userdata = &buffer;
    //打开设备
    if(SDL_OpenAudio(&spec,nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();

        SDL_Quit();
        return;
    }



   //开始播放数据
   //设置播放起始位置
    SDL_PauseAudio(0);

    while (!isInterruptionRequested()) {

        if (buffer.len > 0) {
            continue;
        }
           int bytesPerSample =  SDL_AUDIO_BITSIZE(spec.format) * spec.channels >> 3;
           int simples = buffer.pullLen / bytesPerSample;
           SDL_Delay(simples/spec.freq *1000);
           qDebug() <<simples/spec.freq << "播放结束";
           break;
    }

    SDL_FreeWAV(data);
    //关闭设备
    SDL_CloseAudio();

    //清楚所以子系统
    SDL_Quit();
}
