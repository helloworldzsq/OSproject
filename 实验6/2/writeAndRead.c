#include <stdio.h>
// 为了open()中的O_RDWR
#include <fcntl.h>

// 定义设备进入点（设备名）
#define DEVICE_NAME "/dev/rwbuf"

//调用设备驱动程序
int main()
{
    // 声明文件描述符
    int fd;
    // 记录关闭设备的返回值
    int ret;
    // 写入到设备的内容
    char buff_write[10] = "volume";
    // 读取到设备的结果
    char buff_read[10] = "";

    // 调用打开设备函数。注意O_RDWR是字母O
    fd = open(DEVICE_NAME, O_RDWR);
    // 打开失败
    if (fd == -1)
    {
        printf("[open-error]\n");
        return 0;
    }
    else
    {
        printf("[open-success]\n");
    }

    // 调用驱动程序的写操作接口函数
    if (write(fd, buff_write, 5) == -1)
    {
        printf("[write-error]\n");
        return 0;
    }
    // 写入成功
    else
    {
        printf("[write-success] %s\n", buff_write);
    }

    // 调用驱动程序的读操作接口函数
    if (read(fd, buff_read, 5) == -1)
    {
        printf("[read-error]\n");
        return 0;
    }
    // 读取成功
    else
    {
        buff_read[5] = '\0';
        printf("[read-success] %s\n", buff_read);
    }

    // 设备关闭
    close(fd);
    printf("[close-success]\n");

    return 0;
}
