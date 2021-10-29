#include<stdio.h>
#include <stdlib.h>
int main()
{
int id,fd[2],pid;
char buf[50],s[50];
//创建一个管道 fd[0]:读管道，fd[1]:写管道。
pipe(fd);
while ((id=fork())==-1);
if (id==0)
{   //将后边的字符串写到buf中
	sprintf(buf,"Child1 is sending message!");
	write(fd[1],buf,50);
	//立即终止调用进程
	exit(0);
	}
else
{
    //在获得通知前该线程将一直等待
	wait(0);
	read(fd[0],s,50);
	printf("%s\n",s);
	while ((pid=fork())==-1);
	if (pid==0){
	   	sprintf(buf,"Child2 is sending message!");
	    write(fd[1],buf,50);
	    exit(0);
	}
	else{
		wait(0);
	    read(fd[0],s,50);
    	printf("%s\n",s);
	}
}
return 0;
}
