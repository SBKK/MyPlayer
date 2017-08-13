#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include <QVector>
#include<QMutex>
#include<QWaitCondition>
#include<QDebug>

#include "global.h"
#include "packetqueue.h"
extern "C"{
#include "libavformat/avformat.h"
}


/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
    AVFrame *frame;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int width;
    int height;
    int format;
    AVRational sar;
    int uploaded;
    int flip_v;
} Frame;

class FrameQueue
{
public:
    FrameQueue();
    ~FrameQueue();

    Frame *frame_queue_peek();
    Frame *frame_queue_peek_next();
    Frame *frame_queue_peek_last();
    Frame *frame_queue_peek_writable();
    Frame *frame_queue_peek_readable();
    void frame_queue_push();
    void frame_queue_next();
    int frame_queue_nb_remaining();

    QVector<Frame> queue;
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int rindex_shown;
    QMutex *mutex;
    QWaitCondition  *cond;
    PacketQueue *pktq;



};

#endif // FRAMEQUEUE_H
