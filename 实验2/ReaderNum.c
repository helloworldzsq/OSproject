# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

// wrt(记录型信号量) 用于实现对文件的互斥访问
// mutex 用于对count变量的互斥访问
sem_t wrt, mutex, readerNum;

//记录当前有几个读进程在访问文件
int readCount;

//读者
void* Reader(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&readerNum);
        sem_wait(&mutex);
        readCount++;
        if(readCount == 1)
            sem_wait(&wrt);
        // V操作
        sem_post(&mutex);
        printf("Thread %ld: is reading\n", threadid);
        sleep(3);
        sem_wait(&mutex);
        readCount--;
        if(readCount == 0)
            sem_post(&wrt);
        sem_post(&mutex);
        sem_post(&readerNum);
    }
    //    结束进程
    pthread_exit(0);
}

//写者
void* Writer(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&wrt);
        printf("Thread %ld: is writing\n", threadid);
        sleep(5);
        sem_post(&wrt);
    }
    pthread_exit(0);
}


int main() {
    sem_init(&readerNum,0,5);
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);
    readCount = 0;
    pthread_t tid[10];
    for (int i = 0; i < 6; ++i) {
        pthread_create(&tid[i], NULL ,Reader, (void*)i);
    }
    pthread_create(&tid[7], NULL, Writer, (void*)6);
    pthread_create(&tid[8], NULL, Writer, (void*)7);
    int c=0;
    while (1){
        c = getchar();
        if (c=='q' || c=='Q'){
            for (int i = 0; i < 8; ++i) {
                pthread_cancel(tid[i]);
            }
            break;
        }
    }
    //信号量销毁
    sem_destroy(&mutex);
    sem_destroy(&wrt);
    return 0;
}
