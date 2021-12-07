# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>
//mutex保证存钱和取钱互斥访问
sem_t mutex;
int money = 100;
//存钱
void *saveMoney(void* param) {
    long threadid = (long)param;
    sem_wait(&mutex);
    printf("存钱线程 %ld : 存入金额100\n", threadid);
    money+=100;
    sleep(3);
    sem_post(&mutex);
    printf("存钱线程 %ld : 当前金额为%d\n",threadid,money);
    pthread_exit(0);
}

//取钱
void *withDraw(void* param) {
    long threadid = (long)param;
    sem_wait(&mutex);
    printf("取钱线程 %ld : 取出金额50\n",threadid);
    money-=50;
    sleep(1);
    sem_post(&mutex);
    printf("取钱线程 %ld : 当前金额为%d\n",threadid,money);
    pthread_exit(0);
}

int main() {

    //线程id
    pthread_t tid[4];
    sem_init(&mutex, 0, 1);
    pthread_create(&tid[0], NULL , withDraw, (void*)0);
    pthread_create(&tid[1], NULL , saveMoney, (void*)1);
    pthread_create(&tid[2], NULL , withDraw, (void*)2);
    pthread_create(&tid[3], NULL, saveMoney, (void*)3);
    sleep(10);
    printf("最终余额: %d\n", money);
    //释放进程
    for (int i = 0; i < 4; ++i) {
        pthread_cancel(tid[i]);
    }
    return 0;
}

