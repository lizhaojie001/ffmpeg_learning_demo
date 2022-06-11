#include "yuvplayer.h"
#include "ffmpegs.h"
#include <QPainter>
// 出错了就执行goto RET
#define RET(judge, func) \
if (!judge) { \
        qDebug() << #func << "Error" << SDL_GetError(); \
        return; \
}

extern "C" {
    #include <libavutil/imgutils.h>
}




YUVplayer::YUVplayer(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background:#7F0000");

}

YUVplayer::~YUVplayer()
{
    releaseImage();
    _file.close();
    qDebug() << "~YUVplayer()";
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

}

void YUVplayer::timerEvent(QTimerEvent *event)
{

    long  imageSize = av_image_get_buffer_size(_yuv.fmt,
                                              _yuv.size.width(),
                                              _yuv.size.height(),1);
    char data[imageSize];
    int len = _file.read(data,imageSize);
    if(len > 0) {
        RawVideoFrame in = {
            data,
            _yuv.size.width(),
            _yuv.size.height(),
            AV_PIX_FMT_YUV420P
        };
        RawVideoFrame out = {
            nullptr,
            _yuv.size.width(),
            _yuv.size.height(),
            AV_PIX_FMT_RGB24
        };

        FFmpegs::transformFile(in,out);
        releaseImage();
        _image = new QImage((uchar *)out.pixels,_yuv.size.width(),_yuv.size.height(),QImage::Format_RGB888);
        update();
    }else{
        killTimer(_timerId);
        _state = ::isStoped;
        emit yuvPalyEnd();
        _file.close();
        setYUV(_yuv);
    }
}


void YUVplayer::paintEvent(QPaintEvent *event)
{
    if(!_image) return;
    QPainter painter(this);

    //尺寸最终位置
    int dx = 0;
    int dy = 0;
    int dw = _image->width();
    int dh = _image->height();

    if (width() < _image->width() || height() < _image->height()) {
        if (width() * dh > dw * height()) {
            dw = dw * height() / dh;
            dh = height();
        } else {
            dh = dh * width() / dw;
            dw = width();
        }

    }



    dx = (width() - dw)>>1;
    dy = (height() - dh)>>1;
    painter.drawImage(QRect(dx,dy,dw,dh),*_image);
}

void YUVplayer::releaseImage()
{
    if (_image) {
        free(_image->bits());
        delete _image;
        _image = nullptr;
    }
}
