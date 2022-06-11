#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFile>

#include <SDL2/SDL.h>

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

protected:
void timerEvent(QTimerEvent *event);


private:
    Ui::MainWindow *ui;
    QWidget *_widget;

    QFile _file;

    //展示窗口
    SDL_Window * _window = nullptr;

    //展示上下文
    SDL_Renderer *_rRETerer = nullptr;

    //渲染纹理
    SDL_Texture *_texture = nullptr;

    int _timerId = 0;
};
#endif // MAINWINDOW_H
