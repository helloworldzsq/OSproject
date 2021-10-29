# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>
#define BUFFER_SIZE 5

//empty  同步信号量，表示剩余空间的数量
// full 同步信号量，表示产品的数量
// mutex 互斥信号量，实现对缓冲区的互斥访问
sem_t empty, full, mutex;

typedef int buffer_item;

//缓冲区
buffer_item buffer[BUFFER_SIZE];

int in, out;

// 记录产品的id
int id = 0;

//生产产品
int insert_item(buffer_item item) {
    buffer[out] = item;
    out = (out + 1) % BUFFER_SIZE;
    return 0;
}

//消费产品
int remove_item(buffer_item *item) {
    // 将buffer[in]移除，并将item填充进去
    *item = buffer[in];
    in = (in + 1) % BUFFER_SIZE;
    return 0;
}

//生产者
void *producer(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&empty);
        sem_wait(&mutex);
        //生产产品
        insert_item(id);
        sleep(6);
        printf("ThreadId %ld : Producer produce product %d \n", threadid,id);
        id++;
        sem_post(&mutex);
        sem_post(&full);
    }
    pthread_exit(0);
}

//消费者
void *consumer(void* param) {
    long threadid = (long)param;
    while (1){
        sem_wait(&full);
        sem_wait(&mutex);
        //消费产品
        int item;
        remove_item(&item);
        sleep(3);
        printf("ThreadId %ld : Consumer consume product %d \n", threadid ,item);
        sem_post(&mutex);
        sem_post(&empty);
    }
    pthread_exit(0);
}

int main() {
    //线程id
    pthread_t tid[4];
    //对mutex进行初始化
    //第二个参数 不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享
    //第三个参数 给出了信号量的初始值。　　
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    in = out = 0;
    //两个生产者，两个消费者
    pthread_create(&tid[0], NULL ,consumer, (void*)0);
    pthread_create(&tid[1], NULL ,producer, (void*)1);
    pthread_create(&tid[2], NULL ,consumer, (void*)2);
    pthread_create(&tid[3], NULL, producer, (void*)3);
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
    //释放信号量
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}

