#include "aveventthread.h"

AVEventThread::AVEventThread(AVManager *avmngr,MyOpenGLWidget *gl)
{
    this->avmngr = avmngr;
    this->gl = gl;
}


double AVEventThread::vp_duration(Frame *vp, Frame *nextvp) {
    if (vp->serial == nextvp->serial) {
        double duration = nextvp->pts - vp->pts;
        if (isnan(duration) || duration <= 0 || duration > avmngr->max_frame_duration)
            return vp->duration;
        else
            return duration;
    } else {
        return 0.0;
    }
}

double AVEventThread::compute_target_delay(double delay)
{
    double sync_threshold, diff = 0;

    /* update delay to follow master synchronisation source */
        /* if video is slave, we try to correct big delays by
           duplicating or deleting a frame */
        diff = avmngr->vidclk.get_clock() - avmngr->audclk.get_clock();;

        /* skip or repeat frame. We take into account the
           delay to compute the threshold. I still don't know
           if it is the best guess */
        sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
        if (!isnan(diff) && fabs(diff) < avmngr->max_frame_duration) {
            if (diff <= -sync_threshold)
                delay = FFMAX(0, delay + diff);
            else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                delay = delay + diff;
            else if (diff >= sync_threshold)
                delay = 2 * delay;
        }


    av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
            delay, -diff);

    return delay;
}

void AVEventThread::video_display()
{
    //qDebug()<<"paly one frame";
    if (avmngr->video_st)
        gl->playframe();

}

void AVEventThread::video_refresh(double *remaining_time){

    double time;
    //goto display;
    //avmngr->force_refresh = 1;

    if (avmngr->video_st) {
retry:
        if (avmngr->pictq.frame_queue_nb_remaining() == 0) {
            // nothing to do, no picture to display in the queue
        } else {
            double last_duration, duration, delay;
            Frame *vp, *lastvp;

            /* dequeue the picture */
            lastvp = avmngr->pictq.frame_queue_peek_last();
            vp = avmngr->pictq.frame_queue_peek();

//                if (vp->serial != avmngr->videoq.serial) {
//                    frame_queue_next(&avmngr->pictq);
//                    goto retry;
//                }

//            if (lastvp->serial != vp->serial)
//                avmngr->frame_timer = av_gettime_relative() / 1000000.0;

            if (avmngr->paused)
                goto display;

            /* compute nominal last_duration */
            last_duration = vp_duration(lastvp, vp);
            delay = compute_target_delay(last_duration);

            time= av_gettime_relative()/1000000.0;
            if (time < avmngr->frame_timer + delay) {
                *remaining_time = FFMIN(avmngr->frame_timer + delay - time, *remaining_time);
                goto display;
            }

            avmngr->frame_timer += delay;
            if (delay > 0 && time - avmngr->frame_timer > AV_SYNC_THRESHOLD_MAX)
                avmngr->frame_timer = time;

            avmngr->pictq.mutex->lock();
            if (!isnan(vp->pts))
                /* update current video pts */
                avmngr->vidclk.set_clock(vp->pts, vp->serial);

            avmngr->pictq.mutex->unlock();

//            if (frame_queue_nb_remaining(&avmngr->pictq) > 1) {
//                Frame *nextvp = frame_queue_peek_next(&avmngr->pictq);
//                duration = vp_duration(vp, nextvp);
//                if((framedrop>0 || framedrop) && time > avmngr->frame_timer + duration){
//                    avmngr->frame_drops_late++;
//                    frame_queue_next(&avmngr->pictq);
//                    goto retry;
//                }
//            }

            avmngr->pictq.frame_queue_next();
            avmngr->force_refresh = 1;

//                if (is->step && !is->paused)
//                    stream_toggle_pause(is);
        }
display:
        /* display picture */
        if (avmngr->force_refresh && avmngr->pictq.rindex_shown)
            video_display();
    }
    avmngr->force_refresh = 0;
}


void AVEventThread::run(){
    for(;;){
        double remaining_time = 0.0;
        if (remaining_time > 0.0)
            av_usleep((int64_t)(remaining_time * 1000000.0));
        remaining_time = REFRESH_RATE;
        if ((!avmngr->paused || avmngr->force_refresh))
            video_refresh(&remaining_time);
        //gl->playframe();
//        qDebug()<<"hi";
    }
}

