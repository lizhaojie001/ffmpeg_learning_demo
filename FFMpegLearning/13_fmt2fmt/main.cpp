#include "mainwindow.h"
#include "ffmpegs.h"
#include <QApplication>
extern "C" {
    #include <libavdevice/avdevice.h>
}

#undef main
int main(int argc, char *argv[])
{

    RawVideoFile in = {
        "E:/out.yuv",
        500,500,
        AV_PIX_FMT_YUV420P
    };

    RawVideoFile out = {
        "E:/out_rgb.yuv",
        500,500,
        AV_PIX_FMT_RGB24
    };

    FFmpegs::transformFile(in,out);


    return 0;

    // 初始化Video子系统
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 0;
    }


    qputenv("QT_SCALE_FACTOR", QByteArray("1"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    avdevice_register_all();
    int ret = a.exec();
    SDL_Quit();
    return ret;
}
