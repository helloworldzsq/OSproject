#include<stdio.h>
#include<unistd.h>
void main()
{int p1,p2,i;
 while((p1=fork())==-1);//�����ӽ���p1�������һ�����ɣ�ֱ�������ӽ��̳ɹ�Ϊֹ
  if(p1==0)
   {
     lockf(1,1,0);//������һ������Ϊstdout����׼����豸��������
     //printf("child  %4d %4d\n",getppid(),getpid(),p1);
     for(i=0;i<5;i++)
       printf("daughter%d\n",i);
     lockf(1,0,0); 
	}
  else{
     while((p2=fork())==-1);//�����ӽ���p2
     if(p2==0){
        lockf(1,1,0);//������һ������Ϊstdout����׼����豸��������
        for(i=0;i<5;i++)
         printf("son%d\n",i);
        lockf(1,0,0); //����
        }
      else
      { lockf(1,1,0);//����
        for(i=0;i<5;i++)
         printf("parent%d\n",i);
        lockf(1,0,0); //����
        
      }
    }   
}

