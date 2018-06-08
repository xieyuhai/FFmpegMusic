package com.ocean.ffmpeg;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import java.io.File;

/**
 * 课题：OpenSL ES播放音频原理详解
 * 知识点：
 * Android底层播放音频
 * OpenSL ES的 Object 和 Interface概念
 * OpenSL ES调用FFmpeg获取pcm数据
 * 音频缓冲队列详解
 * 混音器概念与使用
 * AndroidStudio使用OpenSL ES官方配置文档
 * https://developer.android.com/ndk/guides/audio/getting-started.html
 * <p>
 * OpenSLES官网
 * https://www.khronos.org/opensles/
 * <p>
 * 官方文档
 * https://www.khronos.org/files/opensl-es-1-1-quick-reference.pdf
 */

/**
 * 导出anr文件
 * adb pull /data/anr/traces_com.ocean.ffmpeg_07_6月_11_45_12.232.txt /Users/xieyuhai/Desktop/text1111.txt
 * <p>
 * 命令播放pcm纯音频文件 ；
 * /Applications/VLC.app/Contents/MacOS/VLC --demux=rawaud --rawaud-channels 1 --rawaud-samplerate 8000 /Users/xieyuhai/Desktop/test.pcm
 * <p>
 * //音频所占字节数=通道数 * 采用频率（Hz） * 采用位数 （byte）
 * 将mp3转换为pcm
 */
public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btn = findViewById(R.id.player);
        btn.setText(Environment.getExternalStorageDirectory().getAbsolutePath());
    }

    MusicPlayer player = new MusicPlayer();

    public void player(View view) {
        String input = new File(Environment.getExternalStorageDirectory(), "test_cbr.mp3").getAbsolutePath();
        String pcm = new File(Environment.getExternalStorageDirectory(), "test.pcm").getAbsolutePath();


//        player.player(input, pcm);
        player.player();

        Button btn = (Button) view;
        btn.setText("success");
    }

    public void pause(View view) {
        player.pause();
        Button btn = (Button) view;
        btn.setText("success");
    }

    public void replayer(View view) {
        player.replayer();
        Button btn = (Button) view;
        btn.setText("success");
    }

    public void stop(View view) {

        Button btn = (Button) view;
        btn.setText("success");

        player.stop();
    }


    int currentVolume = 1;

    public void setVolume(View view) {
        player.setVolumeAudioPlayer(currentVolume++);
    }
}
