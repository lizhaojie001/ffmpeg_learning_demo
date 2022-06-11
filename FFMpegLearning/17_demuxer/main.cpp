﻿#include "mainwindow.h"

#include <QApplication>
extern "C" {
    #include <libavdevice/avdevice.h>
}
int main(int argc, char *argv[])
{
    qputenv("QT_SCALE_FACTOR", QByteArray("1"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    avdevice_register_all();
    return a.exec();
}
