#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h>
int main()
{
    int fd;
    fd= open("/dev/chardev",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
        ioctl(fd,1,1);
    }
    printf("文件已清空\n");
    return 0;
}
