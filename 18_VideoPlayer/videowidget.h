#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H
#include "videoplayer.h"
#include <QWidget>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

public slots:
    void onPlayerVideoDeceoded(VideoPlayer* p,
                               uint8_t* data,
                               VideoPlayer::SwsVideoSpec &spec);
    void onPlayerVideoStateChanged(VideoPlayer* p);
signals:


private:
    QImage * _image = nullptr;
    QRect _rect;

    void paintEvent(QPaintEvent *event) override;
    void _freeImage();
};

#endif // VIDEOWIDGET_H
