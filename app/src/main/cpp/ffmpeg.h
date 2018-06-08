//
// Created by 谢玉海 on 2018/6/7.
//

#ifndef FFMPEGMUSIC_FFMPEG_H
#define FFMPEGMUSIC_FFMPEG_H

#endif //FFMPEGMUSIC_FFMPEG_H

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

int createFFmpeg(int *rate, int *channel);

int getPcm(void **pcm,size_t* pcm_size);

void realeaseFFmpeg();