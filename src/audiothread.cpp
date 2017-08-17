#include "audiothread.h"

AudioThread::AudioThread(AVManager *avmngr)
{
    this->avmngr = avmngr;

    int ret;
    ret = audio_open(&this->avmngr->audio_tgt);

    this->avmngr->audio_hw_buf_size = ret;
    this->avmngr->audio_src = this->avmngr->audio_tgt;
    this->avmngr->audio_buf = NULL;
    this->avmngr->audio_buf1 = NULL;
    this->avmngr->audio_buf_size  = 0;
    this->avmngr->audio_buf1_size = 0;
    this->avmngr->audio_buf_index = 0;
    this->avmngr->audio_write_buf_size = 0;
}

int AudioThread::audio_open(struct AudioParams *audio_hw_params){

    SDL_AudioSpec wanted_spec, spec;
    int64_t wanted_channel_layout = avmngr->auddec->channel_layout;
    int wanted_nb_channels = avmngr->auddec->channels;
    int wanted_sample_rate = avmngr->auddec->sample_rate;

    if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.freq = wanted_sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = avmngr;

    while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
        av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
               wanted_spec.channels, wanted_spec.freq, SDL_GetError());
        return -1;
    }

    audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
    audio_hw_params->freq = spec.freq;
    audio_hw_params->channel_layout = wanted_channel_layout;
    audio_hw_params->channels = spec.channels;
    audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
    audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
    if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }

    return spec.size;
}

void audio_callback(void *userdata, Uint8 *stream, int len){
    AVManager *avmngr = (AVManager *) userdata;
    int len1, audio_size;

    while(len > 0) {
      if(avmngr->audio_buf_index >= avmngr->audio_buf_size) {
            /* We have already sent all our data; get more */
            audio_size = audio_decode_frame(avmngr);
      if(audio_size < 0) {
            /* If error, output silence */
            avmngr->audio_buf = NULL;
            avmngr->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / avmngr->audio_tgt.frame_size * avmngr->audio_tgt.frame_size;
        } else {
            avmngr->audio_buf_size = audio_size;
        }
            avmngr->audio_buf_index = 0;
      }
      len1 = avmngr->audio_buf_size - avmngr->audio_buf_index;
      if(len1 > len)
        len1 = len;

      memcpy(stream, (uint8_t *)avmngr->audio_buf + avmngr->audio_buf_index, len1);


      len -= len1;
      stream += len1;
      avmngr->audio_buf_index += len1;
    }

    avmngr->audio_write_buf_size = avmngr->audio_buf_size - avmngr->audio_buf_index;
    /* Let's assume the audio driver that is used by SDL has two periods. */
    if (!isnan(avmngr->audio_clock)) {
        avmngr->audclk.set_clock_at(avmngr->audio_clock - (double)(2 * avmngr->audio_hw_buf_size + avmngr->audio_write_buf_size) / avmngr->audio_tgt.bytes_per_sec, avmngr->audio_clock_serial, av_gettime_relative() / 1000000.0);
    }
}

int audio_decode_frame(AVManager *avmngr)
{
    int data_size, resampled_data_size;
    int64_t dec_channel_layout;
    av_unused double audio_clock0;

    Frame *af;

    //get frame;
    if (!(af = avmngr->sampq.frame_queue_peek_readable()))
        return -1;
    avmngr->sampq.frame_queue_next();



    data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(af->frame),
                                           af->frame->nb_samples,
                                           (AVSampleFormat)af->frame->format, 1);

    dec_channel_layout =
        (af->frame->channel_layout && av_frame_get_channels(af->frame) == av_get_channel_layout_nb_channels(af->frame->channel_layout)) ?
        af->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af->frame));


    if (af->frame->format        != avmngr->audio_src.fmt            ||
        dec_channel_layout       != avmngr->audio_src.channel_layout ||
        af->frame->sample_rate   != avmngr->audio_src.freq) {
        swr_free(&avmngr->swr_ctx);
        avmngr->swr_ctx = swr_alloc_set_opts(NULL,
                                         avmngr->audio_tgt.channel_layout, avmngr->audio_tgt.fmt, avmngr->audio_tgt.freq,
                                         dec_channel_layout,(AVSampleFormat)af->frame->format,af->frame->sample_rate,
                                         0, NULL);
        if (!avmngr->swr_ctx || swr_init(avmngr->swr_ctx) < 0) {
            av_log(NULL, AV_LOG_ERROR,
                   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
                    af->frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)af->frame->format), av_frame_get_channels(af->frame),
                    avmngr->audio_tgt.freq, av_get_sample_fmt_name(avmngr->audio_tgt.fmt), avmngr->audio_tgt.channels);
            swr_free(&avmngr->swr_ctx);
            return -1;
        }
        avmngr->audio_src.channel_layout = dec_channel_layout;
        avmngr->audio_src.channels       = av_frame_get_channels(af->frame);
        avmngr->audio_src.freq = af->frame->sample_rate;
        avmngr->audio_src.fmt = (AVSampleFormat)af->frame->format;
    }

    if (avmngr->swr_ctx) {
        const uint8_t **in = (const uint8_t **)af->frame->data;
        uint8_t **out = &avmngr->audio_buf1;
        int out_count = (int64_t)af->frame->nb_samples * avmngr->audio_tgt.freq / af->frame->sample_rate + 256;
        int out_size  = av_samples_get_buffer_size(NULL, avmngr->audio_tgt.channels, out_count, avmngr->audio_tgt.fmt, 0);
        int len2;
        if (out_size < 0) {
            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
            return -1;
        }

        av_fast_malloc(&avmngr->audio_buf1, &avmngr->audio_buf1_size, out_size);
        //qDebug()<<"fast malloc";
        if (!avmngr->audio_buf1)
            return AVERROR(ENOMEM);
        len2 = swr_convert(avmngr->swr_ctx, out, out_count, in, af->frame->nb_samples);
        if (len2 < 0) {
            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
            return -1;
        }
        if (len2 == out_count) {
            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
            if (swr_init(avmngr->swr_ctx) < 0)
                swr_free(&avmngr->swr_ctx);
        }
        avmngr->audio_buf = avmngr->audio_buf1;
        resampled_data_size = len2 * avmngr->audio_tgt.channels * av_get_bytes_per_sample(avmngr->audio_tgt.fmt);
    } else {
        avmngr->audio_buf = af->frame->data[0];
        resampled_data_size = data_size;
    }

    audio_clock0 = avmngr->audio_clock;
    /* update the audio clock with the pts */
    if (!isnan(af->pts))
        avmngr->audio_clock = af->pts + (double) af->frame->nb_samples / af->frame->sample_rate;
    else
        avmngr->audio_clock = NAN;
    avmngr->audio_clock_serial = af->serial;

    return resampled_data_size;
}


void AudioThread::run(){

    SDL_PauseAudio(0);

    int ret = -1;

    AVFrame *frame = av_frame_alloc();
    AVPacket pkt1,*pkt = &pkt1;
    Frame *af;

    if (!frame)
        goto the_end;

    for(;;) {

        avmngr->audioq.deQueue(*pkt);

        /* send the packet with the compressed data to the decoder */
        ret = avcodec_send_packet(avmngr->auddec, pkt);
        if (ret < 0) {
            char s[50];
            av_make_error_string(s,50,ret);
            qDebug()<<s<<"\nError submitting the packet to the decoder";
            goto the_end;
        }

        /* read all the output frames (in general there may be any number of them */
        while (ret >= 0) {
            ret = avcodec_receive_frame(avmngr->auddec, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0) {
                char s[50];
                av_make_error_string(s,50,ret);
                qDebug()<<s<<"\nError during decoding";
                goto the_end;
            }

            if (!(af = avmngr->sampq.frame_queue_peek_writable()))
                goto the_end;

            //put frame
            av_frame_move_ref(af->frame, frame);
            avmngr->sampq.frame_queue_push();


        }
          av_packet_unref(pkt);


    }
the_end:
    qDebug()<<"free pkt";
    av_packet_free(&pkt);
    av_frame_free(&frame);
}










