#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "avmanager.h"

class VideoThread : public QThread
{
public:
    VideoThread(AVManager *avmngr);

    double synchronize(AVFrame *srcFrame, double pts);

    //void run();

    AVManager *avmngr;
};

#endif // VIDEOTHREAD_H
