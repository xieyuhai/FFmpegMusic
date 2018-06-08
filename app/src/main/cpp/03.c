//
// Created by 谢玉海 on 2018/6/7.
//

//
// Created by 谢玉海 on 2018/6/7.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/**
 * 将c文件生成main可执行文件
 * gcc 01.c -o main -lpthread
 */

int i=0;

pthread_mutex_t mutex;

void *thread_fun(void * arg){
    char *no=(char*)arg;
    //加锁
    pthread_mutex_lock(&mutex);
    for(;i<10;i++){
        printf("%s  thread,i :%d\n",no,i);

        sleep(1);
    }
    pthread_mutex_unlock(&mutex);

    i=0;
    return 1;

}

void main(){
    pthread_mutex_init(&mutex,NULL);

    pthread_t tid1,tid2;
    pthread_create(&tid1,NULL,thread_fun,"线程一");
    pthread_create(&tid2,NULL,thread_fun,"线程二");
    void *rval;
    pthread_join(tid1,&rval);
    pthread_join(tid2,&rval);

//生成可执行文件ls

//gcc thread -o main -lpthread
    printf("线程结束%d ",rval);
    printf("线程结束%d ",rval);

    pthread_mutex_destroy(&mutex);
}