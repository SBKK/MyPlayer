#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING,"1");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    //QGridLayout *gridLayout = new QGridLayout(ui->GL);
    //gridLayout->addWidget(gl);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    qDebug()<<event->key();
}

