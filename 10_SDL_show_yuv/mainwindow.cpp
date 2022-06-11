#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiothread.h"
#include <QTimer>
#include <QDebug>

#include "CommonDefine.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_pAudioThread(nullptr)
{

     ui->setupUi(this);

     _widget = new QWidget(this);
     _widget->setGeometry(100,50,512,512);


}

MainWindow::~MainWindow()
{

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (!m_pAudioThread) {
        m_pAudioThread = new AudioThread((void *)_widget->winId(),this);
    }
        m_pAudioThread->start ();
        connect(m_pAudioThread,&AudioThread::finished,[&](){
            m_pAudioThread = nullptr;

         });

}



