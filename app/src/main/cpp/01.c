//
// Created by 谢玉海 on 2018/6/7.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_fun(void *arg) {
    char *no = (char *) arg;
    int i = 0;
    for (; i < 10; i++) {
        printf("%s  thread,i :%d\n", no, i);
        if (i == 5) {
            //线程退出 （自杀）
            pthread_exit(2);
            //他杀pthread_cancel
        }
    }
    return 1;
}


void main() {
    pthread_t tid;
    pthread_create(&tid, NULL, thread_fun, "线程一");
    void *rval;
    pthread_join(tid, &rval);

//生成可执行文件
//gcc thread -o main -lpthread
    printf("线程结束%d ", rval);
}