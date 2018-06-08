package com.ocean.ffmpeg;

import android.content.Context;
import android.os.SystemClock;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() throws Exception {
        assertEquals(4, 2 + 2);
    }


    int ready = 0;

    public void produce() {
        for (; ; ) {
            ready++;
            SystemClock.sleep(1);
        }
    }

    public void consumer() {
        for (; ; ) {

            while (ready == 0) {
                System.out.println("没有产品了 \r\n");
            }
            ready--;
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    public class porduce implements Runnable {
        @Override
        public void run() {
            for (; ; ) {
                synchronized (object) {
                    ready++;
                    System.out.println("生产者 生产" + ready);

                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
//                try {
//                    Thread.sleep(1);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
            }
        }
    }

    public class consumer implements Runnable {
        @Override
        public void run() {
            for (; ; ) {
//                while (ready == 0) {
//                    System.out.println("没有产品了 \r\n");
//                }
                synchronized (object) {
                    ready--;
                    System.out.println("消费者 %s  消费" + ready);

                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

//                try {
//                    Thread.sleep(1);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
            }
        }
    }

    Object object = new Object();

    @Test
    public void Test() {
        new Thread(new porduce()).start();
        new Thread(new consumer()).start();

    }

}