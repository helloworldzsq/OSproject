#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

//ȥ����β���ո�
int trim(char * cmd1,char * tem,char * cmd){
    int m=0;
    while (cmd1[m]==' ')
        m++;
    strncpy(tem,cmd1+m,strlen(cmd1)-m);
    tem[strlen(cmd1)-m] = '\0';
    //�ҵ���һ�����ֿո��λ��
    int n=strcspn(tem ," ");
    if(n>0){
        strncpy(cmd,tem,n);
        cmd[n]='\0';
    }
    return n;
}
int main(){
    char cmd1[80];
    char tem[80];
    char str3[80]={0};
    char cmd[80];
    char *scwt[] = {"dir","cop" ,"era" ,"disp" ,"end"};
    char *scwt2[] = { "dir","cp", "rm" ,"echo","end" };
    static int cmdnum = 5;
    int j;
    //�ӽ��̵ķ�����ֵ
    int rtn;
    while(1){
        printf( "Please input command:");
        gets(cmd1);
        int n = trim(cmd1,tem,cmd);
        if(n>0){
            for (j = 0; j < cmdnum; ++j) {
                if (strcmp(cmd,scwt[j])==0){
                    break;
                }
            }
            //�ڲ�����
            if (j==4){
                exit(0);
            }
            else if (j<cmdnum){
                //ƴ���ַ���
                sprintf(str3,"%s%s",scwt2[j],tem+n);
                system(str3);
                continue;
            } else{
                if (fork()==0){
                    execl("/bin/sh","sh","-c", cmd, (char*)0);
                    perror(cmd);
                }
                wait(0);
            }
        }
    }
    return 0;
}