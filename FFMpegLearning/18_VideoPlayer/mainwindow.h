#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videoplayer.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_BtnFileOpen_clicked();

    void on_BtnPlay_clicked();

    void on_BtnStop_clicked();

    void on_BtnMute_clicked();

    void onPlayStateChanged(VideoPlayer*p);

    void onVideoDuration(VideoPlayer*p);

    void on_SliderVolume_valueChanged(int value);

    void on_SliderCurrent_valueChanged(int value);

private:


    void enableControlUI(bool enable);

    //将微秒转为时间戳
    QString timeText(int second);

private:
    Ui::MainWindow *ui;
    VideoPlayer * _player = nullptr;

};
#endif // MAINWINDOW_H
