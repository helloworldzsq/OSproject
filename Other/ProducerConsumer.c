# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

#define BUFFER_SIZE 5

typedef int buffer_item;

//semaphores
//empty  同步信号量，表示剩余空间的数量
// full 同步信号量，表示产品的数量
// mutex 互斥信号量，实现对缓冲区的互斥访问
sem_t empty, full, mutex;

//buffer
buffer_item buffer[BUFFER_SIZE];

int in, out;
//当前产品的数量
int num=0;
struct data {
    int id;
    //进程创建到开始运行的时间
    int opTime;
//进程运行的时间
    int lastTime;
};

//生产者
void *producer(void* param) {
    int lastTime = ((struct data*)param)->lastTime;
    int opTime = ((struct data*)param)->opTime;
    int id = ((struct data*)param)->id;
    //释放param的空间，存储下一个进程的数据
    free(param);
    while (1){
        sleep(opTime);
        sem_wait(&empty);
        sem_wait(&mutex);
        buffer[out] = 1;
        out = (out + 1) % BUFFER_SIZE;
        num++;
        sleep(lastTime);
        printf("Thread %d: Producer produce product, num = %d\n", id, num);
        sem_post(&mutex);
        sem_post(&full);
    }
    pthread_exit(0);
}

//消费者
void *consumer(void* param) {
    int lastTime = ((struct data*)param)->lastTime;
    int opTime = ((struct data*)param)->opTime;
    int id = ((struct data*)param)->id;
    free(param);
    while (1){
        sleep(opTime);
        sem_wait(&full);
        sem_wait(&mutex);
        /* critical section */
        //remove a item
        buffer[in] = 0;
        in = (in+1)%BUFFER_SIZE;
        num--;
        sleep(lastTime);
        printf("Thread %d: Consumer consume product, num = %d\n", id, num);
        sem_post(&mutex);
        sem_post(&empty);
    }
    pthread_exit(0);
}

int main() {
    //pthread
    pthread_t tid[2]; // the thread identifier

    pthread_attr_t attr; //set of thread attributes

    /* get the default attributes */
    pthread_attr_init(&attr);

    //initial the semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    in = out = 0;

    int id = 0;

    while(scanf("%d", &id) != EOF) {
        char role;      //producer or consumer
        int opTime;     //operating time
        int lastTime;   //run time
        scanf("%c%d%d", &role, &opTime, &lastTime);
        struct data* d = (struct data*)malloc(sizeof(struct data));
        d->id = id;
        d->opTime = opTime;
        d->lastTime = lastTime;
        if(role == 'P') {
            pthread_create(&tid[0], &attr, producer, d);
        }
        else if(role == 'C'){
            pthread_create(&tid[1], &attr, consumer, d);
        }
    }
    sleep(100);
    pthread_cancel(tid[0]);
    pthread_cancel(tid[1]);
    //释放信号量
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}
