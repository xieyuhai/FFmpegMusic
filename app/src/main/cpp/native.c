//
// Created by 谢玉海 on 2018/6/7.
//

#include "ffmpeg.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//android/sdk/platforms/android-9/arch-arm/usr/lib/libOpenSLES.so

SLObjectItf engineObject = NULL;
SLEngineItf engineEngine = NULL;
SLObjectItf outputMixObject = NULL;

//SLEnvironmentalReverbItf
SLEnvironmentalReverbItf reverbItf = NULL;
SLresult sLresult;
SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

//队列缓冲区
SLAndroidSimpleBufferQueueItf bqPlayerQueue = NULL;
//播放器
SLObjectItf bgPlayerObject = NULL;
SLPlayItf bqPlayerPlay = NULL;
//音量对象
SLVolumeItf bqPlayerVolume = NULL;
size_t bufferSize = 0;
void *buffer;


//回调函数，添加pcm数据到缓冲区
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    bufferSize = 0;
    //取到音频数据了
    getPcm(&buffer, &bufferSize);

    if (NULL != buffer && 0 != bufferSize) {
        //播放的关键地方
        SLresult sLresult1 = (*bqPlayerQueue)->Enqueue(bqPlayerQueue, buffer, bufferSize);
        LOGE("正在播放%d ", sLresult1);
    }
}

JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_player(JNIEnv *env, jobject instance) {

    //初始化一个引擎
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //获取到引擎接口
    SLresult sl = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    LOGE("引擎地址%p   SLresult %d  ", engineEngine, sl);

    //创建混音器
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);

    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);

    //设置环境混响
    sLresult = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                                &reverbItf);
    LOGE("播放器接口6 %d", sLresult);
    //设置混音器
    if (SL_RESULT_SUCCESS == sLresult) {
        (*reverbItf)->SetEnvironmentalReverbProperties(
                reverbItf, &settings);
    }
    //    初始化FFmpeg
    int rate;
    int channels;
    createFFmpeg(&rate, &channels);
    LOGE("初始化FFmpeg完毕");
    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    /**
    typedef struct SLDataFormat_PCM_ {
        SLuint32 		formatType;  pcm
        SLuint32 		numChannels;  通道数
        SLuint32 		samplesPerSec;  采样率
        SLuint32 		bitsPerSample;  采样位数
        SLuint32 		containerSize;  包含位数
        SLuint32 		channelMask;     立体声
        SLuint32		endianness;    end标志位
    } SLDataFormat_PCM;
     */
    SLuint32 ch = NULL;
    if (channels == 2) {
        ch = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    } else if (channels == 1) {
        ch = SL_SPEAKER_FRONT_LEFT;
    }

//    SLAndroidDataFormat_PCM_EX
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, channels, rate * 1000/* SL_SAMPLINGRATE_44_1*/,
                            SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                            ch,
                            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource slDataSource = {&android_queue, &pcm};
//    设置混音器
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    LOGE("引擎  %p", engineEngine);
//    混音器关联起来
//创建一个播放器
    sLresult = (*engineEngine)->CreateAudioPlayer(engineEngine, &bgPlayerObject, &slDataSource,
                                                  &audioSnk, 3,
                                                  ids, req);
    LOGE("   sLresult  %d   bgPlayerObject  %p  slDataSource  %p", sLresult, bgPlayerObject,
         slDataSource);
    sLresult = (*bgPlayerObject)->Realize(bgPlayerObject, SL_BOOLEAN_FALSE);
    LOGE(" sLresult  2 %d", sLresult);
//创建一个播放器接口
    sLresult = (*bgPlayerObject)->GetInterface(bgPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    LOGE(" sLresult 3  %d", sLresult);
//    注册缓冲区
    sLresult = (*bgPlayerObject)->GetInterface(bgPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerQueue);
    LOGE(" sLresult 4  %d", sLresult);
//    设置回调接口   getPCm  pthread 函数

    sLresult = (*bqPlayerQueue)->RegisterCallback(bqPlayerQueue, bqPlayerCallback, NULL);
    LOGE(" sLresult  5  %d", sLresult);

    (*bgPlayerObject)->GetInterface(bgPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);

//    播放第一帧
    bqPlayerCallback(bqPlayerQueue, NULL);
}

/**
 * 停止并释放资源
 * @param env
 * @param instance
 */
JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_stop(JNIEnv *env, jobject instance) {

    if (bgPlayerObject != NULL) {
        (*bgPlayerObject)->Destroy(bgPlayerObject);
        bgPlayerObject = NULL;
        bqPlayerPlay = NULL;

        bqPlayerQueue = NULL;
        bqPlayerVolume = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        reverbItf = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }


    realeaseFFmpeg();

}

/**
 * 暂停
 * @param env
 * @param instance
 */
JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_pause(JNIEnv *env, jobject instance) {

    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);

}

/**
 * 重新接着播放
 * @param env
 * @param instance
 */
JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_replayer(JNIEnv *env, jobject instance) {

    // TODO
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);

}


SLVolumeItf getVolume() {
//    if (fdPlayerVolume != NULL)
//        return fdPlayerVolume;
//    else
    return bqPlayerVolume;
}


JNIEXPORT void JNICALL
Java_com_ocean_ffmpeg_MusicPlayer_setVolumeAudioPlayer(JNIEnv *env, jobject instance, jint volume) {

    SLresult result;
    SLVolumeItf volumeItf = getVolume();

    SLmillibel level=volume;

    if (NULL != volumeItf) {
        result = (*volumeItf)->SetVolumeLevel(volumeItf, level);
        (void) result;
    }
}