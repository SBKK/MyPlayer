#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "avmanager.h"
#include "framequeue.h"
#include"myopenglwidget.h"

class VideoThread : public QThread
{
public:
    VideoThread(AVManager *avmngr);
    void video_refresh(double *remaining_time);

    void run();

    AVManager *avmngr;

private:
    double vp_duration(Frame *vp, Frame *nextvp);
    double compute_target_delay(double delay);
    void video_display();
};

#endif // VIDEOTHREAD_H
