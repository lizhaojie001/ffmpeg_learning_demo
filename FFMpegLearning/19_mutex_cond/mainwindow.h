#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SDL2/SDL.h"

#include <QMainWindow>

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
    void on_pushButton_clicked();

private:
    void consumer(QString name);
    void product(QString name);

private:
    Ui::MainWindow *ui;

    SDL_mutex * _mutex = nullptr;
    SDL_cond * _cond1 = nullptr, * _cond2 = nullptr;

    std::list<QString> *_queue = nullptr;

    int _index = 0;
};
#endif // MAINWINDOW_H
