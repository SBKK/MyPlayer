#ifndef AVMANAGER_H
#define AVMANAGER_H

#include "global.h"
#include <QThread>
#include "packetqueue.h"
#include "framequeue.h"

extern "C"{
#include "libavformat/avformat.h"
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

    int video_stream;
    AVStream *video_st;
    PacketQueue videoq;
    AVCodecContext *viddec;
    double video_clock;
    FrameQueue pictq;

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

    int audio_hw_buf_size;
    struct AudioParams audio_src;
    struct AudioParams audio_tgt;

    struct SwrContext *swr_ctx;

    AVFormatContext *ic;
    QWaitCondition *continue_read_thread;

};



#endif // AVMANAGER_H
