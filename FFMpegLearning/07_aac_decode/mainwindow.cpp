#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiothread.h"
#include <QDebug>
#include "playthread.h"
#include "ffmpegs.h"
#include "CommonDefine.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_pAudioThread(nullptr)
{

     ui->setupUi(this);

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
         });
        ui->pushButton->setEnabled(false);

}







