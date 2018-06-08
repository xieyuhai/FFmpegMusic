//
// Created by 谢玉海 on 2018/6/7.
//


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//产品队列
int ready=0;

//互斥锁
pthread_mutex_t mutex;
//条件变量
pthread_cond_t has_product;

//生产者
void *produce(void * arg){

    for(;;){
        pthread_mutex_lock(&mutex);
        ready++;
        printf("生产者  生产  %d ;\r\n",ready);
        sleep(3);

        //发送一条解锁信息
        pthread_cond_signal(&has_product);
        //解锁
        pthread_mutex_unlock(&mutex);
    }
}



//消费者
void *consumer(void * arg){
     char *name=arg;
    for(;;){
        pthread_mutex_lock(&mutex);

        while(ready==0){
            printf("没有产品了 \r\n");
            pthread_cond_wait(&has_product,&mutex);
        }
        ready--;
        printf("消费者 %s  消费 %d ;\r\n",name,ready);

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}


void main() {

    //初始化锁
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&has_product,NULL);

    pthread_t tid1,tid2,tid3;
    pthread_create(&tid1, NULL, produce, "生产者");
    pthread_create(&tid2, NULL, consumer, "消费者1");
    pthread_create(&tid3, NULL, consumer, "消费者2");
    void *rval;
    pthread_join(tid1, &rval);
    pthread_join(tid2, &rval);
    pthread_join(tid3, &rval);

//生成可执行文件
//gcc thread -o main -lpthread
    printf("线程结束%d ", rval);

    pthread_mutex_destroy(&mutex);
}