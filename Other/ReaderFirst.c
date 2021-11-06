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
sem_t wrt, mutex;
//记录当前有几个读进程在访问文件
int readCount;

struct data {
    int id;
//    准备时间
    int opTime;
//    运行时间
    int lastTime;
};

//读者
void* Reader(void* param) {

    int id = ((struct data*)param)->id;
    int lastTime = ((struct data*)param)->lastTime;
    int opTime = ((struct data*)param)->opTime;
    sleep(opTime);
    printf("Thread %d: waiting to read\n", id);
//  P操作，各进程互斥地访问 mutex
    sem_wait(&mutex);
    readCount++;
    if(readCount == 1)
        sem_wait(&wrt);
//  V操作
    sem_post(&mutex);

    printf("Thread %d: start reading\n", id);
    /* reading is performed */
    sleep(lastTime);
    printf("Thread %d: end reading\n", id);

    sem_wait(&mutex);
    readCount--;
    if(readCount == 0)
        sem_post(&wrt);
    sem_post(&mutex);
//    结束进程
    pthread_exit(0);
}

//写者
void* Writer(void* param) {

    int id = ((struct data*)param)->id;
    int lastTime = ((struct data*)param)->lastTime;
    int opTime = ((struct data*)param)->opTime;

    sleep(opTime);
    printf("Thread %d: waiting to write\n", id);
    sem_wait(&wrt);

    printf("Thread %d: start writing\n", id);
    /* writing is performed */
    sleep(lastTime);
    printf("Thread %d: end writing\n", id);
    sem_post(&wrt);
    pthread_exit(0);
}

int main() {
    //pthread_t  线程ID(线程标识符,用于声明线程ID)
    pthread_t tid; // the thread identifier
    //pthread_attr_t  线程属性
    pthread_attr_t attr; //set of thread attributes

    /* get the default attributes */
    //对线程属性进行初始化
    pthread_attr_init(&attr);

    //对mutex进行初始化
    //第二个参数 不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享
    //第三个参数 给出了信号量的初始值。　　
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    readCount = 0;

    int id = 0;
//    EOF 文件结束符  ctrl+z，回车退出
    while(scanf("%d", &id) != EOF) {

        char role;      //producer or consumer
        int opTime;     //operating time
        int lastTime;   //run time

        scanf("%c%d%d", &role, &opTime, &lastTime);
        struct data* d = (struct data*)malloc(sizeof(struct data));

        d->id = id;
        d->opTime = opTime;
        d->lastTime = lastTime;

        if(role == 'R') {
            printf("Create the %d thread: Reader\n", id);
//           创建线程函数
//　         第一个参数为指向线程标识符的指针。第二个参数用来设置线程属性。
//　　       第三个参数是线程运行函数的地址。 最后一个参数是运行函数的参数。
            pthread_create(&tid, &attr, Reader, d);
        }
        else if(role == 'W') {
            printf("Create the %d thread: Writer\n", id);
            pthread_create(&tid, &attr, Writer, d);
        }
    }
    //信号量销毁
    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
