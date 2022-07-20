#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "customslider.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _player = new VideoPlayer();
    connect(_player,&VideoPlayer::playStateChanged,this,&MainWindow::onPlayStateChanged);
    connect(_player,&VideoPlayer::timeChanged,this,&MainWindow::onPlayTimeChanged);

    connect(_player,&VideoPlayer::videoDuration,this,&MainWindow::onVideoDuration);
    connect(_player,&VideoPlayer::playerVideoDeceoded,ui->playerWidget
            ,&VideoWidget::onPlayerVideoDeceoded);
    connect(_player,&VideoPlayer::playStateChanged,ui->playerWidget
            ,&VideoWidget::onPlayerVideoStateChanged);
    connect (ui->SliderCurrent,&CustomSlider::handclicked,this,[&](CustomSlider*s) {
        _player->setTime (s->value ());
    });
    ui->SliderVolume->setValue(50);

    enableControlUI(false);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete _player;

}



void MainWindow::on_BtnFileOpen_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                                                    "选中视频文件",
                                                    "E:/",
                                                    "视频文件(*.mp4 *.mkv *.mp3)");
    if(filename.isEmpty()) return;
    qDebug() << filename;
    enableControlUI(true);
    std::string name = filename.toStdString();
    _player->setFile(name);
    _player->play();
}


void MainWindow::on_BtnPlay_clicked()
{
    if (_player->isPlaying()) {
        _player->pause();
    } else {
        _player->play();
    }
}


void MainWindow::on_BtnStop_clicked()
{
   _player->stop();
}



void MainWindow::onPlayStateChanged(VideoPlayer *p)
{
    if (p->isPlaying()) {
        ui->BtnPlay->setText("暂停");
    } else {
        ui->BtnPlay->setText("播放");
        if (p->state() == VideoPlayer::Stoped) {
            enableControlUI(false);
        }
    }
}

void MainWindow::onPlayTimeChanged(VideoPlayer *p)
{
//    ui->SliderCurrent->blockSignals(true);
    ui->SliderCurrent->setValue(p->getCurrent());
//    ui->SliderCurrent->blockSignals(false);
//    ui->SliderCurrent->update ();
}

void MainWindow::onVideoDuration(VideoPlayer *p)
{
    int second = p->getDuration();
    ui->LabelDuration->setText(timeText(second));
    ui->SliderCurrent->setMaximum(second);
}

void MainWindow::enableControlUI(bool enable)
{
    ui->BtnMute->setEnabled(enable);
    ui->BtnPlay->setEnabled(enable);
    ui->BtnStop->setEnabled(enable);
    ui->SliderCurrent->setEnabled(enable);
    ui->SliderVolume->setEnabled(enable);
    if(!enable) {
        ui->LabelCurrent->setText(timeText(0));
        ui->LabelDuration->setText(timeText(0));
        ui->SliderCurrent->setValue(ui->SliderCurrent->minimum());
        ui->stackedWidget->setCurrentWidget(ui->PlayerFileOpenWidget);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->VideoPlayerWidiget);
    }

}

QString MainWindow::timeText(int second)
{

    int h = second/3600%60;
    int m = second/60%60;
    int s = second%60;
    QString hour = QString("0%1").arg(h).right(2);
    QString min = QString("0%1").arg(m).right(2);
    QString sec = QString("0%1").arg(s).right(2);

    return QString("%1:%2:%3").arg(hour).arg(min).arg(sec);
}


void MainWindow::on_SliderVolume_valueChanged(int value)
{
    ui->LabelVolume->setText(QString("%1").arg(value));
    _player->setVolume(value);
}


void MainWindow::on_SliderCurrent_valueChanged(int value)
{
    ui->LabelCurrent->setText(timeText(value));
}


void MainWindow::on_BtnMute_toggled(bool checked)
{
    ui->BtnMute->setText( checked ? "已静音" : "静音");
    _player->setMute(checked);
}

