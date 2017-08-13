#ifndef READTHREAD_H
#define READTHREAD_H

#include<QThread>

#include "avmanager.h"
#include "audiothread.h"

class ReadThread : public QThread
{
public:
    ReadThread(AVManager *avmngr);

    AVManager *avmngr;

    void run();
};

#endif // READTHREAD_H
