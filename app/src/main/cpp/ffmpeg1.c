//
// Created by 谢玉海 on 2018/6/6.
//

#include <jni.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <android/log.h>
#include <unistd.h>

#define TAG "Ocean"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

/**
 * 将pcm格式数据写入文件
 * @param env
 * @param instance
 * @param input_
 * @param output_
 */

JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_player(JNIEnv *env, jobject instance, jstring input_,
                                         jstring output_) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);
    const char *output = (*env)->GetStringUTFChars(env, output_, 0);

    // TODO
    av_register_all();

    AVFormatContext *pContext = avformat_alloc_context();

    if (avformat_open_input(&pContext, input, NULL, NULL)) {
        LOGE("打开失败");
        return;
    }

    if (avformat_find_stream_info(pContext, NULL) < 0) {
        LOGE("获取信息失败");
        return;
    }

    int audio_stream_index = -1;
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
    AVCodecContext *pCodecContext = pContext->streams[audio_stream_index]->codec;
    //获取解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodecContext->codec_id);


    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {

    }

    //
    AVPacket *packet = av_malloc(packet);
//    av_init_packet()

    //音频数据
    AVFrame *frame = av_frame_alloc();

    //mp3里面所包含的编码格式  转换成pcm
    SwrContext *swrContext = swr_alloc();

    int got_frame;

    //44100*2
    uint8_t *out_buffer = av_malloc(44100 * 2);

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
    int out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

    //输出文件
    FILE *pcmFILE = fopen(output, "wb");

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
                fwrite(out_buffer, 1, size, pcmFILE);
            }

            av_free_packet(packet);
        }
    }

    LOGE("解码完成");
    fclose(pcmFILE);
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecContext);
    avformat_close_input(&pContext);






    (*env)->ReleaseStringUTFChars(env, input_, input);
    (*env)->ReleaseStringUTFChars(env, output_, output);
}
