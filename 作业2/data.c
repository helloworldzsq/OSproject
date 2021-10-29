# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

//empty  同步信号量，表示剩余空间的数量
// full 同步信号量，表示产品的数量
//单个缓冲区可以不设置互斥信号量
sem_t full,empty;

//数据采集
void *get(void* param) {

    long threadid = (long)param;
    while (1){
        sem_wait(&empty);
        printf("ThreadId %ld : collect data\n", threadid);
        sleep(6);
        sem_post(&full);
    }
    pthread_exit(0);
}

//数据计算
void *compute(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&full);
        printf("ThreadId %ld : compute data \n",threadid);
        sleep(3);
        sem_post(&empty);
    }
    pthread_exit(0);
}

int main() {
    //线程id
    pthread_t tid[4];
    //第二个参数 不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享
    //第三个参数 给出了信号量的初始值。　　
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);
    //两个生产者，两个消费者
    pthread_create(&tid[0], NULL , compute, (void*)0);
    pthread_create(&tid[1], NULL , get, (void*)1);
    pthread_create(&tid[2], NULL , compute, (void*)2);
    pthread_create(&tid[3], NULL, get, (void*)3);
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
    return 0;
}

