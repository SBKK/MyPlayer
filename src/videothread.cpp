#include "videothread.h"

VideoThread::VideoThread(AVManager *avmngr)
{
    this->avmngr = avmngr;
}

void VideoThread::run(){

    AVFrame *frame = av_frame_alloc();
    AVPacket pkt1,*pkt = &pkt1;
    double pts;
    double duration;
    int ret;
    AVRational tb = avmngr->video_st->time_base;
    AVRational frame_rate = av_guess_frame_rate(avmngr->ic, avmngr->video_st, NULL);

    if (!frame)
        goto the_end;

    for (;;) {

        avmngr->videoq.deQueue(*pkt);

        /* send the packet with the compressed data to the decoder */
        ret = avcodec_send_packet(avmngr->viddec, pkt);
        if (ret < 0) {
            char s[50];
            av_make_error_string(s,50,ret);
            qDebug()<<s<<"\nError submitting the packet to the decoder";
            goto the_end;
        }

        /* read all the output frames (in general there may be any number of them */
        while (ret >= 0) {
            ret = avcodec_receive_frame(avmngr->viddec, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0) {
                char s[50];
                av_make_error_string(s,50,ret);
                qDebug()<<s<<"\nError during decoding";
                goto the_end;
            }

            duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);
            pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            //ret = queue_picture(is, frame, pts, duration, av_frame_get_pkt_pos(frame), is->viddec.pkt_serial);

            Frame *vp;

            if (!(vp = avmngr->pictq.frame_queue_peek_writable()))
                    break;

            vp->sar = frame->sample_aspect_ratio;
            vp->uploaded = 0;

            vp->width = frame->width;
            vp->height = frame->height;
            vp->format = frame->format;

            vp->pts = pts;
            vp->duration = duration;

            av_frame_move_ref(vp->frame, frame);
            avmngr->pictq.frame_queue_push();
        }
        av_packet_unref(pkt);
    }
 the_end:
    av_frame_free(&frame);
}
