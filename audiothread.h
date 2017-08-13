#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include "global.h"
extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
}
#include "avmanager.h"
#include "framequeue.h"

class AudioThread : public QThread
{
public:
    AudioThread(AVManager *avmngr);

    int audio_open(AudioParams *audio_hw_params);

    int decoder_decode_frame(AVFrame *frame);


    void run();

    AVManager *avmngr;
};

void audio_callback(void *userdata, Uint8 *stream, int len);
int audio_decode_frame(AVManager *avmngr);

#endif // AUDIOTHREAD_H
