/*
* 	写者优先
*/

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>


// RWMutex 读写互斥
// mutex1 readCount互斥
// mutex2 writeCount互斥
// wrt 写者互斥
// mutex3的主要用处就是避免写者同时与多个读者进行竞争，读者中信号量RWMutex比mutex3先释放，则一旦有写者，写者可马上获得资源。
sem_t RWMutex, mutex1, mutex2, mutex3, wrt;
//用于记录正在等待的写者的数目
int writeCount, readCount;


//读者
void* Reader(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&mutex3);
        sem_wait(&RWMutex);
        sem_wait(&mutex2);
        readCount++;
        if(readCount == 1)
            sem_wait(&wrt);
        sem_post(&mutex2);
        sem_post(&RWMutex);
        sem_post(&mutex3);
        sleep(3);
        printf("Thread %ld: is reading\n", threadid);
        sem_wait(&mutex2);
        readCount--;
        if(readCount == 0)
            sem_post(&wrt);
        sem_post(&mutex2);
    }
    pthread_exit(0);
}

//写者
void* Writer(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&mutex1);
        writeCount++;
        if(writeCount == 1){
            sem_wait(&RWMutex);
        }
        sem_post(&mutex1);
        sem_wait(&wrt);
        sleep(5);
        printf("Thread %ld: is writing\n", threadid );
        sem_post(&wrt);
        sem_wait(&mutex1);
        writeCount--;
        if(writeCount == 0) {
            sem_post(&RWMutex);
        }
        sem_post(&mutex1);
    }
    pthread_exit(0);
}

int main() {

    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&mutex3, 0, 1);
    sem_init(&wrt, 0, 1);
    sem_init(&RWMutex, 0, 1);

    readCount = writeCount = 0;
    pthread_t tid[4];
    pthread_create(&tid[0], NULL ,Reader, (void*)0);
    pthread_create(&tid[2], NULL ,Reader, (void*)1);
    pthread_create(&tid[1], NULL ,Writer, (void*)2);
    pthread_create(&tid[3], NULL, Writer, (void*)3);
    int c=0;
    while (1){
        c = getchar();
        if (c=='q' || c=='Q'){
            for (int i = 0; i < 4; ++i) {
                pthread_cancel(tid[i]);
            }
            break;
        }
    }

    sem_destroy(&mutex1);
    sem_destroy(&mutex2);
    sem_destroy(&mutex3);
    sem_destroy(&RWMutex);
    sem_destroy(&wrt);
    return 0;
}
