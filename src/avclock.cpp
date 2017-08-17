#include "avclock.h"

AVClock::AVClock()
{
    paused = 0;
    //this->queue_serial = queue_serial;
    set_clock(NAN, -1);
}

void AVClock::set_clock(double pts, int serial){
    double time = av_gettime_relative() / 1000000.0;
    set_clock_at(pts, serial, time);
}

void AVClock::set_clock_at(double pts, int serial, double time){
    this->pts = pts;
    last_updated = time;
    pts_drift = this->pts - time;
    this->serial = serial;
}

double AVClock::get_clock(){
//    if (*queue_serial != serial)
//            return NAN;
        if (paused) {
            return pts;
        } else {
            double time = av_gettime_relative() / 1000000.0;
            return pts_drift + time;
        }
}

