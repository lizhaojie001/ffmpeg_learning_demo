#include "yuvplayer.h"
// 出错了就执行goto RET
#define RET(judge, func) \
if (!judge) { \
        qDebug() << #func << "Error" << SDL_GetError(); \
        return; \
}

extern "C" {
    #include <libavutil/imgutils.h>
}

static const std::map<AVPixelFormat,SDL_PixelFormatEnum> sdl_texture_format_map = {
    { AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
    { AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
    { AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
    { AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
    { AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
    { AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
    { AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
    { AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
    { AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
    { AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
    { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
    { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
    { AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
    { AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
    { AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
    { AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
    { AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
    { AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
    { AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
    { AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
 };


YUVplayer::YUVplayer(QWidget *parent)
    : QWidget{parent}
{

    _window = SDL_CreateWindowFrom((void *)winId());
    RET(_window,SDL_CreateWindow);


    //创建rRETerer上下文

    //开启硬件加速
    _rRETerer = SDL_CreateRenderer(_window, -1,SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
    if(!_rRETerer) {
        _rRETerer = SDL_CreateRenderer(_window  ,-1,0);
        RET(_rRETerer,SDL_CreateRenderer);
    }


}

YUVplayer::~YUVplayer()
{
    _file.close();
    SDL_DestroyRenderer(_rRETerer);
    SDL_DestroyTexture(_texture);
    SDL_DestroyWindow(_window);
}

void YUVplayer::play()
{

    _timerId = startTimer(1000/_yuv.fps);
    _state = ::isPlaying;

}

void YUVplayer::pause()
{
    killTimer(_timerId);
    _state = ::isPaused;

}

void YUVplayer::stop()
{
    killTimer(_timerId);
    _state = ::isStoped;

}

YUVPlayState YUVplayer::state()
{
    return _state;
}

void YUVplayer::setYUV(YUV yuv)
{
    _yuv = yuv;
    _file.setFileName(yuv.filename);
    if(!_file.open(QFile::ReadOnly)){
        qDebug() << "file open error";
        return;
    }

    //创建纹理
    SDL_PixelFormatEnum pixformat = sdl_texture_format_map.find(yuv.fmt)->second;
    _texture = SDL_CreateTexture(_rRETerer,pixformat
                                 ,SDL_TEXTUREACCESS_STREAMING,
                                 yuv.size.width(),yuv.size.height());
    RET(_texture,SDL_CreateTexture);
}

void YUVplayer::timerEvent(QTimerEvent *event)
{

    long  imageSize = av_image_get_buffer_size(_yuv.fmt,
                                              _yuv.size.width(),
                                              _yuv.size.height(),1);
    char data[imageSize];
    int len = _file.read(data,imageSize);
    if(len > 0) {
        SDL_UpdateTexture(_texture,
                          nullptr,
                          (void*)data,_yuv.size.width());

        //设置画笔颜色
        RET((SDL_SetRenderDrawColor(_rRETerer,0,0,0,255) == 0),SDL_SetRenderDrawColor);

        //清楚背景色
        RET((SDL_RenderClear(_rRETerer) == 0),SDL_RenderClear);

        //将纹理复制到渲染目标上 window
        RET((SDL_RenderCopy(_rRETerer,_texture,nullptr,nullptr) == 0),SDL_RenderCopy);

        //展示纹理

        SDL_RenderPresent(_rRETerer);

    }else{
        killTimer(_timerId);
        _state = ::isStoped;
        emit yuvPalyEnd();
        _file.close();
        SDL_DestroyTexture(_texture);
        setYUV(_yuv);
    }
}
