#include "playthread.h"
#include <QtDebug>
#include <QFile>
#include "CommonDefine.h"
extern "C" {
    #include <SDL2/SDL.h>
}

#ifdef Q_OS_WIN
#define SIMPLERATE 44100
#define SIMPLE_SIZE 16
#define CHANNELS  2
#define SIMPLES 1024
#define BYTES_PER_SIMPLE  (SIMPLE_SIZE * CHANNELS) / 8
#define BUFFER_SIZE  (BYTES_PER_SIMPLE * SIMPLES)

#else

#endif

//全局变量默认会初始化
int buffer_length;
char * buffer_data;

//子线程
//音频设备会多次回调
void pull_data_AudioCallback(void *userdata,
                           //需要填充的数据PCM
                           Uint8 * stream,
                           //希望填充的样本大小
                           int len)
{
  //希望填充的样本大小
  //是根据设置的 (simples 1024) 样本数  x 每个样本的大小(16位 x 2声道数 = 32bit /8 = 4byte) = 4096byte = 4.096kb

    //清空steam;
    SDL_memset(stream,0,len);

    if (buffer_length <= 0) {
        return;
    }
    //防止指针越界
    len = (len >buffer_length ) ? buffer_length : len;
    //填充数据
    SDL_MixAudio(stream,(Uint8 *)buffer_data,len,SDL_MIX_MAXVOLUME);
    //
    buffer_data+=len;
    buffer_length -= len;
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
    int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (ret != 0) {
        const char * err = SDL_GetError();
        qDebug() << err ;
        SDL_Quit();
        return;

    }
    SDL_AudioSpec spec;
    //采样率
    spec.freq = SIMPLERATE;
    //采样格式
    spec.format = AUDIO_S16LSB;
    //声道数
    spec.channels = CHANNELS;
    //音频缓冲区样本数  值为2的n次幂
    spec.samples = SIMPLES;
    //回调
    spec.callback = pull_data_AudioCallback;


    //打开设备
    if(SDL_OpenAudio(&spec,nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();

        SDL_Quit();
        return;
    }

    //打开文件
    QFile file(FILENAME);
   if (!file.open(QFile::ReadOnly)) {
        qDebug() <<"打开文件失败";
        //关闭设备
        SDL_CloseAudio();

        SDL_Quit();
        return;
    }

   //开始播放数据

   //设置播放起始位置
    SDL_PauseAudio(0);
    //从文件读取的数据
    char data[BUFFER_SIZE];
    while (!isInterruptionRequested()) {

        if (buffer_length > 0) {
            continue;
        }

       int len = file.read(data,BUFFER_SIZE);
       buffer_length = len;
       //文件读取完毕
       if (buffer_length <= 0) {
           break;
       }
       buffer_data = data;


    }

    file.close();
    //关闭设备
    SDL_CloseAudio();

    //清楚所以子系统
    SDL_Quit();
}
