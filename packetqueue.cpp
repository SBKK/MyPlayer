#include "packetqueue.h"

PacketQueue::PacketQueue() :
    queue(new QQueue<AVPacket>),
    abort_request(0),
    mutex(new QMutex),
    cond(new QWaitCondition)
{

}

PacketQueue::~PacketQueue(){
    delete queue;
    delete mutex;
    delete cond;
}

bool PacketQueue::enQueue(const AVPacket &pkt){

    mutex->lock();

    if (abort_request)
           return false;

    queue->enqueue(pkt);
    //qDebug()<<"PACKET enqueue:"<<queue->size();
    cond->wakeOne();
    mutex->unlock();
    return true;
}

bool PacketQueue::deQueue(AVPacket &pkt){

    bool flag;

    mutex->lock();

    for(;;){

        if (abort_request) {
           flag = false;
           break;
       }

        if(!queue->empty()){
            pkt = queue->head();
            queue->dequeue();   
            //qDebug()<<"PACKET dequeue:"<<queue->size();
            flag = true;
            break;
        }else{
            cond->wait(mutex);
        }
    }
    mutex->unlock();
    return flag;

}

