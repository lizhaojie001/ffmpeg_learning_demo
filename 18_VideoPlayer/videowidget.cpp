#include "videowidget.h"
#include <QPainter>
VideoWidget::VideoWidget(QWidget *parent)
    : QWidget{parent}
{

}

VideoWidget::~VideoWidget()
{
  _freeImage();

}

void VideoWidget::onPlayerVideoDeceoded(VideoPlayer *p,
                                        uint8_t *data,
                                        VideoPlayer::SwsVideoSpec &spec)
{
    if (p->state() == VideoPlayer::Stoped) return;
    _freeImage();
    _image = new QImage((uchar *)data,spec.width,spec.height,QImage::Format_RGB888);
    update();
}

void VideoWidget::onPlayerVideoStateChanged(VideoPlayer *p)
{
    if (p->state() == VideoPlayer::Stoped) {
        _freeImage();
        update();
    }
}

void VideoWidget::paintEvent(QPaintEvent *event)
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

void VideoWidget::_freeImage()
{
    if(_image) {
        free(_image->bits());
        delete _image;
        _image = nullptr;
        update();
    }

}

