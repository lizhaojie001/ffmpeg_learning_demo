#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //默认创建的是双向队列
    _queue = new std::list<QString>();
    _mutex = SDL_CreateMutex();
    _cond1 = SDL_CreateCond();
    _cond2 = SDL_CreateCond();


    //创建消费者
    consumer("消费者1");
    consumer("消费者2");
    consumer("消费者3");

}

MainWindow::~MainWindow()
{
    delete ui;
    SDL_DestroyCond(_cond1);
    SDL_DestroyCond(_cond2);
    SDL_DestroyMutex(_mutex);
}

void MainWindow::consumer(QString name)
{
    std::thread thread([this,name]() {
        SDL_LockMutex(_mutex);
            //消费
        while(true) {
            qDebug() << name << "消费数据";
            while (!_queue->empty()) {
                qDebug() << _queue->front();
                //弹出队头
                _queue->pop_front();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            qDebug() << name << "进入等待";
            SDL_CondBroadcast(_cond2);
            SDL_CondWait(_cond1,_mutex);
        }
        SDL_UnlockMutex(_mutex);

    });
    thread.detach();
}

void MainWindow::product(QString name)
{
    std::thread thread([this,name]() {
        SDL_LockMutex(_mutex);
        while (true) {
            if (_queue->size() < 100) {
                qDebug() << name << "生产数据";
                _queue->push_back(QString("%1").arg(_index++));
                _queue->push_back(QString("%1").arg(_index++));
                _queue->push_back(QString("%1").arg(_index++));
                SDL_CondBroadcast(_cond1);
            }
            SDL_CondWait(_cond2,_mutex);
        }


        SDL_UnlockMutex(_mutex);

    });
    thread.detach();
}


void MainWindow::on_pushButton_clicked()
{

    product("生产者1");
    product("生产者2");
    product("生产者3");
    product("生产者4");
}

