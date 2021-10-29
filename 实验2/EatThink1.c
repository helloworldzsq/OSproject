# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

//mutex互斥的取筷子
sem_t chopstick[5],mutex;
void *eat_think(void *arg)
{
    int phi = (int)arg;
    while (1){
        sem_wait(&mutex);
        sem_wait(&chopstick[phi]);       //拿左
        sem_wait(&chopstick[(phi+1)%5]); //拿右
        printf("Philosopher %d fetches chopstick %d\n",phi,phi);
        printf("Philosopher %d fetches chopstick %d\n",phi,phi+1);
        sem_post(&mutex);
        sleep(5); //吃饭
        printf("Philosopher %d is eating \n",phi);
        sem_post(&chopstick[phi]);
        sem_post(&chopstick[phi+1]);
        printf("Philosopher %d release chopstick %d\n", phi, phi);
        printf("Philosopher %d release chopstick %d\n", phi, phi+1);
        //思考
        sleep(3);
    }
}
int main() {
    //线程id
    pthread_t tid[5];

    sem_init(&mutex,0,1);
    for (int i = 0; i < 5; ++i) {
        sem_init(&chopstick[i],0,1);
    }
    //    sem_init(&mutex, 0, 1);
    for (int i = 0; i < 5; ++i) {
        pthread_create(&tid[i] ,NULL, eat_think,(void *)i);
    }
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

