
/*read_dev.c  使用用户设备驱动程序的示例*/
#include <stdio.h>
#include <linux/fs.h>
main()
{
    int i,j,num;
    int f;
    char buffer[80];
    f=open("/dev/mychardev",0);
    printf("How many chars do you read:(1-80)");
    scanf("%d",&num);
    i=read(f,buffer,num);
    j=0;
    while (j<i){
        printf("%c",buffer[j]);
        j++;}
    close(f);
    printf("\n");
}