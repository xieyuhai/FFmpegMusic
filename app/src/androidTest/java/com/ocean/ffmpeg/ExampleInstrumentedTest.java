package com.ocean.ffmpeg;

import android.content.Context;
import android.os.SystemClock;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static java.lang.Thread.sleep;
import static org.junit.Assert.*;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void useAppContext() throws Exception {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        assertEquals("com.ocean.ffmpeg", appContext.getPackageName());
    }


    int ready = 0;

    public void produce() {
        for (; ; ) {
            ready++;

            SystemClock.sleep(1);
        }


    }

    public class porduce implements Runnable {

        @Override
        public void run() {
            for (; ; ) {
                ready++;

                SystemClock.sleep(1);
            }

        }
    }

    public class consumer implements Runnable {
        @Override
        public void run() {
            for (; ; ) {

                while (ready == 0) {
                    System.out.println("没有产品了 \r\n");
                }
                ready--;
                SystemClock.sleep(1000);
            }
        }
    }

    public void consumer() {
        for (; ; ) {

            while (ready == 0) {
                System.out.println("没有产品了 \r\n");
            }
            ready--;
            SystemClock.sleep(1000);
        }
    }


    @Test
    public void Test() {
        new Thread(new porduce()).start();
        new Thread(new consumer()).start();

    }
}
