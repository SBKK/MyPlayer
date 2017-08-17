#ifndef AVMANAGER_H
#define AVMANAGER_H

#include "global.h"
#include <QThread>
#include "packetqueue.h"
#include "framequeue.h"
#include<QWidget>
#include"avclock.h"



extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "SDL2/SDL.h"
}

typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

class AVManager
{
public:
    AVManager(const char *filename);
    int open_codec(AVCodecContext **avctx, AVCodecParameters *codecpar);
    void run();

    int force_refresh;
    int paused;

    double frame_timer;
    double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity

    AVClock audclk;
    AVClock vidclk;

    int video_stream;
    AVStream *video_st;
    PacketQueue videoq;
    AVCodecContext *viddec;
    double video_clock;
    FrameQueue pictq;

    double audio_clock;
    int audio_clock_serial;
    int audio_stream;
    AVStream *audio_st;
    PacketQueue audioq;
    AVCodecContext *auddec;
    FrameQueue sampq;
    uint8_t *audio_buf;
    uint8_t *audio_buf1;
    unsigned int audio_buf_size; /* in bytes */
    unsigned int audio_buf1_size; /* in bytes */
    unsigned int audio_buf_index; /* in bytes */
    int audio_write_buf_size;

    int audio_hw_buf_size;
    struct AudioParams audio_src;
    struct AudioParams audio_tgt;

    struct SwrContext *swr_ctx;

    AVFormatContext *ic;
    QWaitCondition *continue_read_thread;

};



#endif // AVMANAGER_H
