#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiothread.h"
#include <QTimer>
#include <QDebug>
#include "playthread.h"

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
    }
    m_pPlayThread->start();
}
