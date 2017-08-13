#include "framequeue.h"

FrameQueue::FrameQueue() :
    queue(FRAME_QUEUE_SIZE),
    rindex(0),
    windex(0),
    size(0),
    max_size(FRAME_QUEUE_SIZE),
    keep_last(1),
    rindex_shown(0),
    mutex(new QMutex),
    cond(new QWaitCondition),
    pktq(NULL)

{
    for(QVector<Frame>::iterator it = queue.begin();it!=queue.end();it++){
        it->frame = av_frame_alloc();
    }


}

FrameQueue::~FrameQueue(){

    for(QVector<Frame>::iterator it = queue.begin();it!=queue.end();it++){
        av_frame_unref(it->frame);
        av_frame_free(&it->frame);
    }
}

Frame *FrameQueue::frame_queue_peek()
{
    return &queue[(rindex + rindex_shown) % max_size];
}

Frame *FrameQueue::frame_queue_peek_next()
{
    return &queue[(rindex + rindex_shown + 1) % max_size];
}

Frame *FrameQueue::frame_queue_peek_last()
{
    return &queue[rindex];
}

Frame *FrameQueue::frame_queue_peek_writable()
{
    /* wait until we have space to put a new frame */
    mutex->lock();
    while (size >= max_size &&
           !pktq->abort_request) {
        cond->wait(mutex);
    }
    mutex->unlock();

    if (pktq->abort_request)
        return NULL;

    return &queue[windex];
}

Frame *FrameQueue::frame_queue_peek_readable()
{
    /* wait until we have a readable a new frame */
    mutex->lock();
    while (size - rindex_shown <= 0 &&
           !pktq->abort_request) {
        cond->wait(mutex);
    }
    mutex->unlock();

    if (pktq->abort_request)
        return NULL;

    return &queue[(rindex + rindex_shown) % max_size];
}

void FrameQueue::frame_queue_push()
{
    if (++windex == max_size)
        windex = 0;
    mutex->lock();
    size++;
    cond->wakeOne();
    mutex->unlock();
}

void FrameQueue::frame_queue_next()
{
    if (keep_last && !rindex_shown) {
        rindex_shown = 1;
        return;
    }
    av_frame_unref(queue[rindex].frame);
    if (++rindex == max_size)
        rindex = 0;
    mutex->lock();
    size--;
    cond->wakeOne();
    mutex->unlock();
}

/* return the number of undisplayed frames in the queue */
int FrameQueue::frame_queue_nb_remaining()
{
    return size - rindex_shown;
}


