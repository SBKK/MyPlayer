#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    AVManager *avmngr = new AVManager("C:\\CloudMusic\\杨宗纬 - 越过山丘.mp3");
    //AVManager *avmngr = new AVManager("C:\\Users\\wy\\Downloads\\Girl's Day\\Ring My Bell-Girl's Day(1080p-60fps).mp4");

    ReadThread *read_thread = new ReadThread(avmngr);
    read_thread->start();

    AudioThread *audio_thread = new AudioThread(avmngr);
    audio_thread->start();



}

MainWindow::~MainWindow()
{
    delete ui;
}
