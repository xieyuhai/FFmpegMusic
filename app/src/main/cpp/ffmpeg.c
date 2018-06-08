//
// Created by 谢玉海 on 2018/6/6.
//

#include "ffmpeg.h"

AVFormatContext *pContext;
AVCodecContext *pCodecContext;
AVCodec *pCodec;
AVPacket *packet;
AVFrame *frame;

//mp3里面所包含的编码格式  转换成pcm
SwrContext *swrContext;
uint8_t *out_buffer;
int out_channel_nb;

int audio_stream_index = -1;//

//opensl  es调用 int *rate, int *channel
int createFFmpeg(int *rate, int *channel) {

//    char *input = "/mnt/sdcard/test_cbr.mp3";
    char *input = "/mnt/sdcard/input.mp3";

//    char *input = "/storage/emulated/0/test.mp3";
    // TODO
    av_register_all();

    pContext = avformat_alloc_context();

    if (avformat_open_input(&pContext, input, NULL, NULL)) {
        LOGE("打开失败");
    }

    if (avformat_find_stream_info(pContext, NULL) < 0) {
        LOGE("获取信息失败");
        return -1;
    }


    //找到音频流
    for (int i = 0; i < pContext->nb_streams; ++i) {

        LOGE("循环  %d", i);
        //codec 每一个流对应的解码上下文，codec_type流的类型
        if (pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }

    //mp3的解码器 获取到解码器上下文
    pCodecContext = pContext->streams[audio_stream_index]->codec;
    //获取解码器
    pCodec = avcodec_find_decoder(pCodecContext->codec_id);


    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {

    }

    //
    packet = av_mallocz(sizeof(AVPacket));
//    av_init_packet(packet);

    //音频数据
    frame = av_frame_alloc();

    //mp3里面所包含的编码格式  转换成pcm
    swrContext = swr_alloc();



    //44100*2
    out_buffer = av_malloc(44100 * 2);

    /**
     * struct SwrContext *swr_alloc_set_opts(struct SwrContext *s,
                                      int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
                                      int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
                                      int log_offset, void *log_ctx);
     */
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;//立体声
    //输出采样位数 16位
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输出的采样率必须与输入相同
    int out_sample_rate = pCodecContext->sample_rate;


    swr_alloc_set_opts(swrContext,
                       out_ch_layout,
                       out_sample_fmt,
                       out_sample_rate,
                       pCodecContext->channel_layout,
                       pCodecContext->sample_fmt,
                       pCodecContext->sample_rate,
                       0,
                       NULL
    );

    //音频所占字节数=通道数 * 采用频率（Hz） * 采用位数 （byte）
    swr_init(swrContext);

    //获取通道数
    out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

    *rate = pCodecContext->sample_rate;
    *channel = pCodecContext->channels;


    return 0;
}

//pcm入参出参
int getPcm(void **pcm, size_t *pcm_size) {

    int frameCount = 0;
    int got_frame;

    while (av_read_frame(pContext, packet) >= 0) {
        if (packet->stream_index == audio_stream_index) {
//解码   mp3 编码格式 frame转  pcm
            avcodec_decode_audio4(pCodecContext, frame, &got_frame, packet);

            if (got_frame) {
                LOGE("解码");
//                int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
//                                const uint8_t **in , int in_count);
                swr_convert(swrContext,
                            &out_buffer,
                            44100 * 2,
                            frame->data,
                            frame->nb_samples);

                //通道数
                int size = av_samples_get_buffer_size(NULL,
                                                      out_channel_nb,
                                                      frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);

                *pcm = out_buffer;
                *pcm_size = size;
                break;
            }
        }
    }
    return 0;
}


void realeaseFFmpeg() {
    av_free_packet(packet);
    av_free(out_buffer);
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecContext);
    avformat_close_input(&pContext);
}