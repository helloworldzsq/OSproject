// 模块
#include <linux/module.h>
// 内核
#include <linux/kernel.h>
// struct file_operations
#include <linux/fs.h>
// copy_to_user() & copy_from_user
#include <linux/uaccess.h>
// <string.h>不行，用于memset和strcpy
#include <linux/string.h>
// kmalloc和kfree
#include <linux/slab.h>
// rwbuf.c,  driver for virtual char-device
#define RW_CLEAR 0x123
// 设备名
#define DEVICE_NAME "rwbuf"
// buffer数组的最大长度
#define RWBUF_MAX_SIZE 1024
//网址:https://blog.csdn.net/sandalphon4869/article/details/105900817
//设备驱动程序
// 设备中存储信息的全局结构体指针
typedef struct Data
{
    // 用于打开和释放的验证，0表示未使用，1表示已使用
    int count;
    // 表示字符串
    char buffer[RWBUF_MAX_SIZE];
} Data, *DataPtr;

DataPtr myDataPtr = NULL;

// 设备的打开：只返回0表示成功无法表示报错
int rwbuf_open(struct inode *inode, struct file *file){
    // 如果未被使用
    if (myDataPtr == NULL)
    {
        // 分配堆
        myDataPtr = (DataPtr)kmalloc(sizeof(Data), GFP_KERNEL);
        // 字符数组清零
        memset(myDataPtr->buffer, 0, sizeof(myDataPtr->buffer));
        // 初始化为1,表示有人在使用了
        myDataPtr->count = 1;
        printk("[rwbuf_open-kmalloc-success]myDataPtr->count = %d.\n", myDataPtr->count);
        return 0;
    }
    // 已被使用
    else
    {
        // 增加一个用户
        myDataPtr->count += 1;
        printk("[rwbuf_open-countup]myDataPtr->count = %d.\n", myDataPtr->count);
        return 0;
    }
}

// 设备的关闭：返回-1表示错误；返回0表示成功
int rwbuf_release(struct inode *inode, struct file *file){
    // 当未打开时,报错
    if (myDataPtr == NULL)
    {
        printk("[rwbuf_release-error]\n");
        return -1;
    }
    // 当有用户使用时,则自减
    else
    {
        myDataPtr->count -= 1;
        printk("[rwbuf_release-countdown]myDataPtr->count = %d.\n", myDataPtr->count);

        // 当没用用户使用时,释放
        if (myDataPtr->count == 0)
        {
            // 释放堆区
            kfree(myDataPtr);
            // 还要让指针回归NULL，不要野指针
            myDataPtr = NULL;
            printk("[rwbuf_release-free-success]\n");
            return 0;
        }
        return 0;
    }
}

// 设备的读操作：返回-1表示错误；返回(0, RWBUF_MAX_SIZE]表示成功
ssize_t rwbuf_read(struct file *file, char *buf, size_t count, loff_t *f_pos){
    // 判断读取的长度是否有效
    if (strlen(myDataPtr->buffer) > 0 && strlen(myDataPtr->buffer) <= RWBUF_MAX_SIZE)
    {
        // 从内核空间rwbuf复制到用户空间buf
        copy_to_user(buf, myDataPtr->buffer, count);
        printk("[rwbuf_read-success]the size of myDataPtr->buffer after read = %d\n", strlen(myDataPtr->buffer));
        return count;
    }
    else
    {
        printk("[rwbuf_read-error] strlen(myDataPtr->buffer) = %d\n", strlen(myDataPtr->buffer));
        return -1;
    }
}

// 设备的写操作接：返回-1表示错误；返回(0, RWBUF_MAX_SIZE]表示成功
ssize_t rwbuf_write(struct file *file, const char *buf, size_t count, loff_t *f_pos){
    // 判断写入的长度是否有效
    if (count > 0 && count <= RWBUF_MAX_SIZE)
    {
        // 从用户空间buf复制到内核空间rwbuf
        copy_from_user(myDataPtr->buffer, buf, count);
        printk("[rwbuf_write-success] the size of myDataPtr->buffer after write = %d\n", strlen(myDataPtr->buffer));
        return count;
    }
    else
    {
        printk("[rwbuf_write-error] length of string = %d\n", count);
        return -1;
    }
}

// 设备的ioctl：返回-1表示错误；返回0表示成功
long rwbuf_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    printk("[RW_CLEAR:%x],[cmd:%x]\n", RW_CLEAR, cmd);
    // 命令
    if (cmd == RW_CLEAR)
    {
        // 表示清零
        memset(myDataPtr->buffer, 0, sizeof(myDataPtr->buffer));
        printk("[rwbuf_ioctl-success] the size of myDataPtr->buffer after ioctl = %d\n", strlen(myDataPtr->buffer));
        return 0;
    }
    // 无此命令时
    else
    {
        printk("[rwbuf_ioctl-error] the size of myDataPtr->buffer after ioctl = %d\n", strlen(myDataPtr->buffer));
        return -1;
    }
}

// rwbuf_fops要在rwf_buf_init()前面声明，因为register_chrdev()函数要使用到
static struct file_operations rwbuf_fops = {
        open : rwbuf_open,
        release : rwbuf_release,
        read : rwbuf_read,
        write : rwbuf_write,
        unlocked_ioctl : rwbuf_ioctl
};

// module_init()内的初始化函数：返回-1表示错误；返回0表示成功
static int __init rwbuf_init(void)
{
    // 表示注册成功与否：-1表示失败，0表示成功（同register_chrdev返回值）。初始化为-1
    int ret = -1;
    /*
    参数1：设备的种类，即主设备号
    参数2：设备的名称
    参数3：和VFS对接的接口，即上面的结构体变量
    */
    ret = register_chrdev(60, DEVICE_NAME, &rwbuf_fops);
    // 注册失败
    if (ret == -1)
    {
        printk("[rwbuf_init-register-failed]\n");
    }
    // 注册成功
    else
    {
        printk("[rwbuf_init-register-success]\n");
    }
    // 返回ret（同register_chrdev返回值）
    return ret;
}

// module_exit()内的退出函数。
static void __exit rwbuf_exit(void)
{
    unregister_chrdev(60, DEVICE_NAME);
    printk("[rwbuf_exit-unregister-success]\n");
}

// 内核模块入口,相当于main()函数,完成模块初始化
module_init(rwbuf_init);
// 卸载时调用的函数入口,完成模块卸载
module_exit(rwbuf_exit);
// GPL协议证书
MODULE_LICENSE("GPL");
