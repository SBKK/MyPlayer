#ifndef AVEVENTTHREAD_H
#define AVEVENTTHREAD_H

#include<QThread>
#include"avmanager.h"
#include"myopenglwidget.h"

class AVEventThread : public QThread
{
public:
    AVEventThread(AVManager *avmngr,MyOpenGLWidget *gl);
    void run();

    double vp_duration(Frame *vp, Frame *nextvp);
    double compute_target_delay(double delay);
    void video_display();
    void video_refresh(double *remaining_time);

    AVManager *avmngr;
    MyOpenGLWidget *gl;
};

#endif // AVEVENTTHREAD_H
