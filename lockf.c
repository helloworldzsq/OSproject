#include<stdio.h>
#include<unistd.h>
void main()
{int p1,p2,i;
 while((p1=fork())==-1);//创建子进程p1，这个是一个技巧，直到创建子进程成功为止
  if(p1==0)
   {
     lockf(1,1,0);//加锁第一个参数为stdout（标准输出设备描述符）
     //printf("child  %4d %4d\n",getppid(),getpid(),p1);
     for(i=0;i<5;i++)
       printf("daughter%d\n",i);
     lockf(1,0,0); 
	}
  else{
     while((p2=fork())==-1);//创建子进程p2
     if(p2==0){
        lockf(1,1,0);//加锁第一个参数为stdout（标准输出设备描述符）
        for(i=0;i<5;i++)
         printf("son%d\n",i);
        lockf(1,0,0); //解锁
        }
      else
      { lockf(1,1,0);//加锁
        for(i=0;i<5;i++)
         printf("parent%d\n",i);
        lockf(1,0,0); //解锁
        
      }
    }   
}

