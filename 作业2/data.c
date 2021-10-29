# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

//empty  ͬ���ź�������ʾʣ��ռ������
// full ͬ���ź�������ʾ��Ʒ������
//�������������Բ����û����ź���
sem_t full,empty;

//���ݲɼ�
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

//���ݼ���
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
    //�߳�id
    pthread_t tid[4];
    //�ڶ������� ��Ϊ��ʱ���ź����ڽ��̼乲������ֻ��Ϊ��ǰ���̵������̹߳���
    //���������� �������ź����ĳ�ʼֵ������
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);
    //���������ߣ�����������
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

