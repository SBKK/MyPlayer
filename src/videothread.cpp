#include "videothread.h"

VideoThread::VideoThread(AVManager *avmngr)
{
    this->avmngr = avmngr;
}

//void VideoThread::run(){

//    AVFrame *frame = av_frame_alloc();
//    AVPacket pkt1, *packet = &pkt1;
//    double pts;
//    int ret;

//    if (!frame) {
//        return AVERROR(ENOMEM);
//    }

//    for (;;) {
//        if(avmngr->videoq.deQueue(packet)<0)
//              break;

//        ret = avcodec_send_packet(avmngr->viddec, packet);
//        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
//            continue;

//        ret = avcodec_receive_frame(avmngr->viddec, frame);
//        if (ret < 0 && ret != AVERROR_EOF)
//            continue;

//        if ((pts = av_frame_get_best_effort_timestamp(frame)) == AV_NOPTS_VALUE)
//            pts = 0;

//        pts *= av_q2d(avmngr->video_st->time_base);

//        pts = synchronize(frame, pts);

//        ret = avmngr->(is, frame);
//        av_frame_unref(frame);


//    }

//    av_frame_free(&frame);
//    return 0;
//}

//double VideoThread::synchronize(AVFrame *srcFrame, double pts)
//{
//    double frame_delay;

//    if (pts != 0)
//        avmngr->video_clock = pts; // Get pts,then set video clock to it
//    else
//        pts = avmngr->video_clock; // Don't get pts,set it to video clock

//    frame_delay = av_q2d(avmngr->video_st->codec->time_base);
//    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

//    avmngr->video_clock += frame_delay;

//    return pts;
//}
