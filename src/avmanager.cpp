#include "avmanager.h"

AVManager::AVManager(const char *filename)
{
    continue_read_thread = new QWaitCondition();

    ic = NULL;
    swr_ctx = NULL;


    pictq.pktq = &videoq;
    sampq.pktq = &audioq;

    audio_clock_serial = -1;
    paused = 0;


    /* register all codecs, demux and protocols */
    av_register_all();
    avformat_network_init();

    /* open input file, and allocate format context */
    if (avformat_open_input(&ic, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", filename);
        exit(1);
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(ic, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    /* dump input information to stderr */
    //av_dump_format(ic, 0, "src_filename", 0);

    //find stream and its ID
    video_stream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream < 0) {
        fprintf(stderr, "Could not find %s stream \n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO));

    }else{
        video_st = ic->streams[video_stream];
        open_codec(&viddec,video_st->codecpar);
    }

    audio_stream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream < 0) {
        fprintf(stderr, "Could not find %s stream \n",
                av_get_media_type_string(AVMEDIA_TYPE_AUDIO));

    }else{
        audio_st = ic->streams[audio_stream];
        open_codec(&auddec,audio_st->codecpar);
    }
}

int AVManager::open_codec(AVCodecContext **avctx,AVCodecParameters *codecpar){
    int ret;
    AVCodec *dec = NULL;

    /* find decoder for the stream */
    dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        fprintf(stderr, "Failed to find %s codec\n",
                av_get_media_type_string(codecpar->codec_type));
        return AVERROR(EINVAL);
    }
    /* Allocate a codec context for the decoder */
    *avctx = avcodec_alloc_context3(dec);
    if (!*avctx) {
        fprintf(stderr, "Failed to allocate the %s codec context\n",
                av_get_media_type_string(codecpar->codec_type));
        return AVERROR(ENOMEM);
    }
    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(*avctx, codecpar)) < 0) {
        fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                av_get_media_type_string(codecpar->codec_type));
        return ret;
    }
    /* Init the decoders */
    if ((ret = avcodec_open2(*avctx, dec, NULL)) < 0) {
        fprintf(stderr, "Failed to open %s codec\n",
                av_get_media_type_string(codecpar->codec_type));
        return ret;
    }

    return 0;
}

