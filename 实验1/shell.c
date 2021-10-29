#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//去除字符串首尾空格
void trim(char * cmdl,char * tem){
    int m=0 , n=0;
    while (cmdl[m]==' ')
        m++;
    if (m==strlen(cmdl))
        return;
    for (int i = strlen(cmdl)-1; i >=0 ; i--) {
        if (cmdl[i]==' ')
            n++;
        else
            break;
    }
    tem[strlen(cmdl)-m-n] = '\0';
    strncpy(tem,cmdl+m,strlen(cmdl)-m-n);
}

void find_cmd(char * cmdl, char * str, char * pre,char * cmd){
    strncpy(pre,cmdl,4);
    pre[4] = '\0';
    if (strcmp(pre,"end")==0)
        exit(0);
    else if (strcmp(pre,"dir")==0)
        system("ls");
    else if (strcmp(pre,"cop ")==0){
        strcpy(cmd,"cp");
        str[strlen(cmdl)-3]='\0';
        strncpy(str,cmdl+3,strlen(cmdl)-3);
        strncat(cmd,str,strlen(str));
        system(cmd);
    }
    else if (strcmp(pre,"era ")==0){
        strcpy(cmd,"rm");
        str[strlen(cmdl)-3]='\0';
        strncpy(str,cmdl+3,strlen(cmdl)-3);
        strncat(cmd,str,strlen(str));
        system(cmd);
    }
    else if (strcmp(pre,"disp")==0){
        strcpy(cmd,"echo");
        str[strlen(cmdl)-4]='\0';
        strncpy(str,cmdl+4,strlen(cmdl)-4);
        strncat(cmd,str,strlen(str));
        system(cmd);
    }
    else
        printf("输入命令错误\n");
}

void main()
{
    char cmdl[80];
    char tem[80];
    char str[80];
    char pre[5];
    char cmd[80];
    while(1)
    {
        printf("Please input command: ");
        gets(cmdl);	//取命令行输入
        trim(cmdl,tem);
        int n;
        //取命令部分
        n=strcspn(tem,"");
        if (n>0)
        {
            find_cmd(tem,str,pre,cmd);
        }
    }
}


