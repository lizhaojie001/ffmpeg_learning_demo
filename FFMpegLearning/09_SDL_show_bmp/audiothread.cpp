#include "audiothread.h"
#include  <QDebug>
#include <QFile>
#include <QTime>
#include "CommonDefine.h"
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

#include <SDL2/SDL.h>


// 出错了就执行goto end
#define END(judge, func) \
    if (!judge) { \
        qDebug() << #func << "Error" << SDL_GetError(); \
        goto end; \
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
    //定义变量

    //展示窗口
    SDL_Window * window = nullptr;

    //展示上下文
    SDL_Renderer *renderer = nullptr;

    //像素数据
    SDL_Surface * surface = nullptr;
    //渲染纹理
    SDL_Texture *texture = nullptr;

    int width = 0;
    int height = 0;

    // 初始化Video子系统
    END((SDL_Init(SDL_INIT_VIDEO) == 0), SDL_Init);


    //读取bmp像素
     surface = SDL_LoadBMP(FILENAME);
     END(surface,SDL_LoadBMP);

    width = 500;
    height =(int)(surface->h * 1.0/ surface->w * width);
    //创建展示窗口
    window = SDL_CreateWindow("展示窗口",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN);
    END(window,SDL_CreateWindow);


    //创建renderer上下文

    renderer = SDL_CreateRenderer(window, -1 ,
                                  //开启硬件加速
                                  SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
   if(!renderer) {
     renderer = SDL_CreateRenderer(window,-1,0);
     END(renderer,SDL_CreateRenderer);
   }


    //创建纹理
    texture = SDL_CreateTextureFromSurface(renderer,surface);
    END(texture,SDL_CreateTextureFromSurface);


    //设置画笔颜色
    END((SDL_SetRenderDrawColor(renderer,255,0,0,255) == 0),SDL_SetRenderDrawColor);

    //清楚背景色
    END((SDL_RenderClear(renderer) == 0),SDL_RenderClear);

//将纹理复制到渲染目标上 window

   END((SDL_RenderCopy(renderer,texture,nullptr,nullptr) == 0),SDL_RenderCopy);

    //展示纹理

    SDL_RenderPresent(renderer);

    SDL_Delay(2000);
end:
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);


}
