#ifndef GLOBAL
#define GLOBAL



#define __STDC_CONSTANT_MACROS
#define SDL_MAIN_HANDLED

extern "C"{
#include "libavformat/avformat.h"
}

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE,VIDEO_PICTURE_QUEUE_SIZE)


#endif // GLOBAL

