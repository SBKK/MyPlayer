#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include "global.h"
extern "C"{
#include "libavformat/avformat.h"
}

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <iostream>
#include <QDebug>


class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();

    bool enQueue(const AVPacket &pkt);
    bool deQueue(AVPacket &pkt);

    QQueue<AVPacket> *queue;
    int abort_request;
    QMutex *mutex;
    QWaitCondition *cond;


};

#endif // PACKETQUEUE_H
