#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//去除首部空格
void trim(char * cmd1,char * tem){
    int m=0;
    while (cmd1[m]==' ')
        m++;
    strncpy(tem,cmd1+m,strlen(cmd1)-m);
    tem[strlen(cmd1)-m] = '\0';
}
int main(){
    char cmd1[80];
    char str2[80];
    char tem[80];
    char str3[80]={0};
    char *cmd;
    char *scwt[] = {"dir","cop" ,"era" ,"disp" ,"end"};
    char *scwt2[] = { "dir","cp", "rm" ,"echo","end" };
    static int cmdnum = 5;
    int j;
    while(1){
        printf( "Please input command:");
        gets(cmd1);
        trim(cmd1,tem);
        //找到第一个出现空格的位置
        int n=strcspn(tem ," ");
        if(n>0 || strlen(tem)>0){
            strncpy(str2,tem,n);
            str2[n]='\0';
            cmd=str2;
            for (j = 0; j < cmdnum; ++j) {
                if (strcmp(cmd,scwt[j])==0){
                    break;
                }
            }
            if (j==4)
                exit(0);
            if (j<cmdnum){
                //拼接字符串
                sprintf(str3,"%s%s",scwt2[j],tem+n);
                system(str3);
                continue;
            }
            printf("Bad command\n");
        }
    }
    return 0;
}