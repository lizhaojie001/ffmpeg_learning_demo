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
signals:


private:
    QImage * _image = nullptr;
    QRect _rect;

    void paintEvent(QPaintEvent *event) override;
};

#endif // VIDEOWIDGET_H
