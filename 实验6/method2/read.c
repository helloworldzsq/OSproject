#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
int main()
{
    int fd,i;
    char msg[1025];
    fd= open("/dev/chardev",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
        for(i=0;i<1025;i++)
            msg[i]='\0';
        read(fd,msg,1024);
        printf("%s\n",msg);
    }
    else
    {
        printf("device open failure,%d\n",fd);
    }
    return 0;
}
