
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_KEY 24
int shmid;
int *addr;
char msg[1024];
void client(){
    shmid = shmget(SHM_KEY,1024,0666|IPC_CREAT);
    addr = shmat(shmid,0,0);
    for (int i = 9; i >=0 ; i--) {
        while (*addr!=-1);
        printf("(client)send\n");
        *addr = i;
    }
}

void server(){
    shmid = shmget(SHM_KEY,1024,0666|IPC_CREAT);
    addr = shmat(shmid,0,0);
    *addr = -1;
    do {
        while (*addr == -1);
        printf("(server)receive\n");
        if (*addr!=0){
            *addr=-1;
        }
    } while (*addr);
    wait(0);
    shmctl(shmid,IPC_RMID,0);
}
int main(){
    int p1;
    while((p1=fork())==-1);
    if (p1==0){
        server();
    }
    else{
        client();
    }
    return 0;
}