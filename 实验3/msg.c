#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#define MSG_KEY 24
//消息队列标识符
int msqid;

struct msgbuf{
    //    消息类型
    long mtype;
    //    缓冲区大小
    char mtext[1024];
}msgbuf;

void client(){
    msqid=msgget(MSG_KEY,0666|IPC_CREAT);
    printf("Input a line:");
    scanf("%s",msgbuf.mtext);
    msgbuf.mtype=1;
    //将消息输入到缓冲区中
    msgsnd(msqid,&msgbuf,1024,0);
}

void server(){
    msqid=msgget(MSG_KEY,IPC_CREAT|0666);
    //从缓冲区中取出消息
    msgrcv(msqid,&msgbuf,1024,2L,0);
    printf("The reversed line is:%s\n",msgbuf.mtext);
    //删除消息队列
    msgctl(msqid,IPC_RMID,0);
}

int main()
{
    int p1;
    while((p1=fork())==-1);
    if (p1==0){
        server();
        wait(0);
    }
    else{
        client();
    }
    return 0;
}