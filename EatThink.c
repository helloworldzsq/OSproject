#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
//筷子作为mutex
//加锁(lock)后，别人就无法打开，只有当锁没有关闭(unlock)的时候才能访问资源

pthread_mutex_t chopstick[5];

void *eat_think(void *arg)
{
    char phi = *(char *)arg;
    int left = phi-'A',right = (phi- 'A'+1)%5; //左右筷子的编号
    while (1){
        sleep(5); //思考
        //对左手的筷子加锁（阻塞操作）
        pthread_mutex_lock(&chopstick[left]);
        printf("Philosopher %d fetches chopstick %d\n", phi, left);
        /*   pthread_mutex_trylock
         试图加锁（不阻塞操作）
         当互斥锁空闲时将占有该锁；否则立即返回
         当锁已经在使用的时候，返回为EBUSY，而不是挂起等待。
         **/
        if (pthread_mutex_trylock(&chopstick[right]) == EBUSY){
            //如果右边筷子被拿走放下左手的筷子
            pthread_mutex_unlock(&chopstick[left]);
            printf("Philosopher %d release chopstick %d\n", phi, left);
            continue;
        }
        //这里之所以不用对右手的筷子加锁，是因为pthread_mutex_trylock 已经加过锁了
        //pthread_mutex_lock(&chopstick[right]); //拿起右手的筷子，如果想观察死锁，把上一句if注释掉，再把这一句的注释去掉
        printf("Philosopher %d fetches chopstick %d\n", phi, right);
        printf("Philosopher %d is eating.\n",phi);
        sleep(5); //吃饭
        pthread_mutex_unlock(&chopstick[left]); //放下左手的筷子
        printf("Philosopher %d release chopstick %d\n", phi, left);
        pthread_mutex_unlock(&chopstick[right]); //放下左手的筷子
        printf("Philosopher %d release chopstick %d\n", phi, right);
    }
}

int main(){
    pthread_t A,B,C,D,E; //5个哲学家
    for (int i = 0; i < 5; i++){
        pthread_mutex_init(&chopstick[i],NULL);
    }
    pthread_create(&A,NULL, eat_think, 'A');
    pthread_create(&B,NULL, eat_think, 'B');
    pthread_create(&C,NULL, eat_think, 'C');
    pthread_create(&D,NULL, eat_think, 'D');
    pthread_create(&E,NULL, eat_think, 'E');
    char c;
    //用户输入q，结束进程
    if (scanf("%c",&c)=='q'){
        pthread_cancel(A);
        pthread_cancel(B);
        pthread_cancel(C);
        pthread_cancel(D);
        pthread_cancel(E);
    }
    return 0;
}