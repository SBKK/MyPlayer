#include "readthread.h"

ReadThread::ReadThread(AVManager *avmngr)
{
    this->avmngr = avmngr;
}

void ReadThread::run(){

    int ret;
    AVPacket pkt1,*pkt=&pkt1;
    QMutex *wait_mutex = new QMutex();

    while (1) {

        /* if the queue are full, no need to read more */
        if (avmngr->audioq.queue->size()*sizeof(AVPacket) + avmngr->videoq.queue->size()*sizeof(AVPacket) > MAX_QUEUE_SIZE
            || (avmngr->audioq.queue->size()>MIN_FRAMES || avmngr->videoq.queue->size()>MIN_FRAMES)) {
            /* wait 10 ms */
            wait_mutex->lock();
            avmngr->continue_read_thread->wait(wait_mutex,10);
            wait_mutex->unlock();
            continue;
        }

        ret = av_read_frame(avmngr->ic, pkt);
        if (ret < 0) {
        if ((ret == AVERROR_EOF || avio_feof(avmngr->ic->pb))) {
            qDebug()<<"end of file";
            break;
        }
//            if (is->video_stream >= 0)
//                packet_queue_put_nullpacket(&is->videoq, is->video_stream);
//            if (is->audio_stream >= 0)
//                packet_queue_put_nullpacket(&is->audioq, is->audio_stream);
//            if (is->subtitle_stream >= 0)
//                packet_queue_put_nullpacket(&is->subtitleq, is->subtitle_stream);
//            is->eof = 1;



            if (avmngr->ic->pb && avmngr->ic->pb->error)
                break;
            wait_mutex->lock();
            avmngr->continue_read_thread->wait(wait_mutex,10);
            wait_mutex->unlock();
            continue;
        }

        if (pkt->stream_index == avmngr->audio_stream) {
            avmngr->audioq.enQueue(*pkt);
        } else if (pkt->stream_index == avmngr->video_stream) {
            //avmngr->videoq.enQueue(pkt);
            av_packet_unref(pkt);
        }else{
            av_packet_unref(pkt);
        }
    }
}

