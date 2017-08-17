#ifndef AVCLOCK_H
#define AVCLOCK_H

#include "global.h"
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/time.h"
}

class AVClock
{
public:
    AVClock();
    double get_clock();
    void set_clock_at(double pts, int serial, double time);
    void set_clock(double pts, int serial);


    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    //int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
};

#endif // AVCLOCK_H
