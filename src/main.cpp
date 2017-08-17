#include "mainwindow.h"
#include <QApplication>

#include "avmanager.h"
#include "readthread.h"
#include "audiothread.h"
#include "videothread.h"
#include "aveventthread.h"
#include"myopenglwidget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();


    //AVManager *avmngr = new AVManager("C:\\CloudMusic\\杨宗纬 - 越过山丘.mp3");
    AVManager *avmngr = new AVManager("C:\\Users\\wy\\Downloads\\Girl's Day\\Ring My Bell-Girl's Day(1080p-60fps).mp4");

    //AVManager *avmngr = new AVManager("cuc_ieschool.flv");

    MyOpenGLWidget *gl = new MyOpenGLWidget(0,false,avmngr);
    gl->show();

    ReadThread *read_thread = new ReadThread(avmngr);
    read_thread->start();

    AudioThread *audio_thread = new AudioThread(avmngr);
    audio_thread->start();

    VideoThread *video_thread = new VideoThread(avmngr);
    video_thread->start();

    AVEventThread *event_loop = new AVEventThread(avmngr,gl);
    event_loop->start();

    return a.exec();
}


