#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
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

    void paintEvent(QPaintEvent *event);

private:
    void releaseImage();
private:
    YUVPlayState _state = ::isPaused;
    YUV _yuv;

    int _timerId;

    QFile _file;

    QImage * _image = nullptr;

};

#endif // YUVPLAYER_H
