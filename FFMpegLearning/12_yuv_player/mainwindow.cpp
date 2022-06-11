
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>

extern "C" {
// 工具相关API（比如错误处理）
#include <libavutil/avutil.h>
// 格式相关API
#include <libavformat/avformat.h>

#include <libavutil/imgutils.h>
}


#include "CommonDefine.h"

#define IMA_W 1280
#define IMA_H 720

#define AV_FMT  AV_PIX_FMT_YUV420P

// 出错了就执行goto RET
#define RET(judge, func) \
    if (!judge) { \
        qDebug() << #func << "Error" << SDL_GetError(); \
        return; \
    }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

     ui->setupUi(this);
    _player = new YUVplayer(this);
    int x = 50;
    int y = 50;
    int w = this->width() - 50*2;
    int h = this->height() - 50*2;
    _player->setGeometry(x,y,w,h);

    //设置yuv数据

    YUV yuv;
#ifdef Q_OS_MAC
    yuv.filename = QString("/Users/macbookpro/Desktop/out.yuv");
    yuv.size = QSize(1920,1028);

#else
    yuv.filename = QString("E:/outBD1280.yuv");
    yuv.size = QSize(1280,720);

#endif
        yuv.fmt = AV_PIX_FMT_YUV420P;
    yuv.fps = 30;

    _player->setYUV(yuv);


    connect(_player,&YUVplayer::yuvPalyEnd,[=]{
        ui->pushButton->setText("播放");
    });

}

MainWindow::~MainWindow()
{

    delete ui;

}


void MainWindow::on_pushButton_clicked()
{
    if (_player->state() != ::isPlaying) {
        _player->play();
        ui->pushButton->setText("暂停");
    } else {
        _player->pause();
        ui->pushButton->setText("播放");
    }
}
