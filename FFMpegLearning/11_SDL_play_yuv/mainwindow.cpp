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

     _widget = new QWidget(this);
     _widget->setGeometry(100,50,910,512);



     int width = 0;
     int height = 0;

     _file.setFileName(FILENAME);

     if(!_file.open(QFile::ReadOnly)){
         qDebug() << "file open error";
         return;
     }
     // 初始化Video子系统
     RET((SDL_Init(SDL_INIT_VIDEO) == 0), SDL_Init);



     _window = SDL_CreateWindowFrom((void *)_widget->winId());
     RET(_window,SDL_CreateWindow);


     //创建rRETerer上下文

     //开启硬件加速
     _rRETerer = SDL_CreateRenderer(_window, -1,SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
    if(!_rRETerer) {
      _rRETerer = SDL_CreateRenderer(_window  ,-1,0);
      RET(_rRETerer,SDL_CreateRenderer);
    }


     //创建纹理
     _texture = SDL_CreateTexture(_rRETerer,SDL_PIXELFORMAT_IYUV
                                 ,SDL_TEXTUREACCESS_STREAMING,
                                 IMA_W,IMA_H);
     RET(_texture,SDL_CreateTexture);
}

MainWindow::~MainWindow()
{

    delete ui;
    _file.close();
    SDL_DestroyRenderer(_rRETerer);
    SDL_DestroyTexture(_texture);
    SDL_DestroyWindow(_window);
}


void MainWindow::on_pushButton_clicked()
{
       _timerId= startTimer(30);
}

void MainWindow::timerEvent(QTimerEvent *event) {

    long  imageSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                             IMA_W,
                                             IMA_H,1);
    char data[imageSize];

    int len = _file.read(data,imageSize);
    if(len > 0) {
    SDL_UpdateTexture(_texture,
                      nullptr,
                      (void*)data,IMA_W);

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
    }
}

