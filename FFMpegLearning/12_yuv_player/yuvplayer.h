#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
#include <SDL2/SDL.h>
#include <QFile>
extern "C" {
   #include <libavformat/avformat.h>
}
enum YUVPlayState
{
    isPlaying,
    isPaused,
    isStoped
};

struct YUV
{
    QString filename;
    AVPixelFormat fmt;
    QSize size;
    int fps = 30;
    YUV() {}
};
class YUVplayer : public QWidget
{
    Q_OBJECT
public:
    explicit YUVplayer(QWidget *parent = nullptr);
    ~YUVplayer();

    void play();
    void pause();
    void stop();
    YUVPlayState state();


    void setYUV(YUV yuv);
signals:
    void yuvPalyEnd();

protected:
    void timerEvent(QTimerEvent *event);
private:
    YUVPlayState _state = ::isPaused;
    YUV _yuv;

    int _timerId;

    QFile _file;

    //展示窗口
    SDL_Window * _window = nullptr;

    //展示上下文
    SDL_Renderer *_rRETerer = nullptr;

    //渲染纹理
    SDL_Texture *_texture = nullptr;
};

#endif // YUVPLAYER_H
