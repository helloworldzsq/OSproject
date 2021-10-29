# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

//定义一信号量s，初始值为20。s>0表示可继续进入售票厅的人数;
// S=0表示售票厅中已有20名顾客（购票者) ;
sem_t s;

void* Ticket(void* param) {
    long ticketid = (long)param;
    while (1){
        sem_wait(&s);
        printf("Customer %ld: is buying\n", ticketid);
        sleep(3);
        sem_post(&s);
    }
    pthread_exit(0);
}

int main() {

    sem_init(&s,0,20);
    pthread_t tid[30];
    for (int i = 0; i < 30 ; ++i) {
        pthread_create(&tid[i], NULL , Ticket , (void*)i);
    }
    int c=0;
    while (1){
        c = getchar();
        if (c=='q' || c=='Q'){
            for (int i = 0; i < 30; ++i) {
                pthread_cancel(tid[i]);
            }
            break;
        }
    }
    return 0;
}
