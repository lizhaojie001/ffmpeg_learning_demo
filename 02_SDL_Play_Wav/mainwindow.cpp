#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiothread.h"
#include <QTimer>
#include <QDebug>
#include "playthread.h"
#include "ffmpegs.h"
#include "CommonDefine.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_pAudioThread(nullptr)
    ,m_pPlayThread(nullptr)
{

     ui->setupUi(this);
     ui->pushButton_2->setEnabled(false);

}

MainWindow::~MainWindow()
{

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (!m_pAudioThread) {
        m_pAudioThread = new AudioThread(this);
    }
        m_pAudioThread->start ();
        connect(m_pAudioThread,&AudioThread::finished,[&](){
            m_pAudioThread = nullptr;
            ui->pushButton->setEnabled(true);
            ui->pushButton_2->setEnabled(false);
         });
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);
        if (m_pTimer) {
         m_pTimer->stop();
         m_pTimer = nullptr;
        }
        QTimer * timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer,&QTimer::timeout,this,[=](){
            time += 1;

            ui->LabTime->setText(QString::fromStdString(std::to_string(time)) + "s");
        });
        timer->start();
        m_pTimer = timer;
}



void MainWindow::on_pushButton_2_clicked()
{
    if (m_pAudioThread) {
        m_pAudioThread->requestInterruption();
        m_pAudioThread = nullptr;
    }
    if (m_pTimer) {
        m_pTimer->stop();
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    if (!m_pPlayThread) {
       m_pPlayThread = new PlayThread(this);
       m_pPlayThread->start();
       ui->pushButton_3->setText("停止播放");
       connect(m_pPlayThread,&PlayThread::finished,this,[=](){
          ui->pushButton_3->setText("播放");
          m_pPlayThread = nullptr;
       });
    } else {
        m_pPlayThread->requestInterruption();
        m_pPlayThread->deleteLater();
        m_pPlayThread = nullptr;
        ui->pushButton_3->setText("播放");
    }
}

void MainWindow::on_pushButton_4_clicked()
{
        WAVHeader header;
#ifdef Q_OS_WIN
        header.NumChannels = 2;
        header.SampleRate = 44100;
        header.BitsPerSample = 16;
#else
        header.NumChannels = 1;
        header.SampleRate = 48000;
        header.BitsPerSample = 32;
        header.AudioFormat = 3; //f32le 对应的格式是0x0003
        //参考地址  http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
#endif

        FFmpegs::pcm2wav(header,FILENAME,OUTWAVFILENAME);
}


void MainWindow::on_pushButton_5_clicked()
{

}

