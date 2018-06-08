//
// Created by 谢玉海 on 2018/6/7.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int i=0;
void *thread_fun(void * arg){
    char *no=(char*)arg;
    //int i=0;
    for(;i<10;i++){
        printf("%s  thread,i :%d\n",no,i);

        sleep(1);
    }
    i=0;
   return 1;
}

void main(){
    pthread_t tid1,tid2;
    pthread_create(&tid1,NULL,thread_fun,"线程一");
    pthread_create(&tid2,NULL,thread_fun,"线程二");
    void *rval;
    pthread_join(tid1,&rval);
    pthread_join(tid2,&rval);

//生成可执行文件
//gcc thread -o main -lpthread
    printf("线程结束%d ",rval);
    printf("线程结束%d ",rval);
}