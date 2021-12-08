/* mychardev.c  创建一个只读的文件系统 */
#ifndef MODULE
#define MODULE
#endif
//Redhat 9下　#include <linux/devfs_fs_kernel.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/init.h>
#include <asm/timex.h>
/* 字符设备所需*/
#include <linux/fs.h>
#include <linux/wrapper.h>  /* 为兼容后续版本*/
//#include <linux/malloc.h>
#include <linux/slab.h>
#include <asm/system.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <asm/segment.h>

#define SUCCESS 0

/* 设备定义 **********************/
/* 设备名, 它将出现在 /proc/devices */
#define DEVICE_NAME "mychardev"

/* 该设备最大信息长度 */
#define BUF_LEN 80

/* 设备否打开?利用它防止当前进程使用同一设备*/
static int Device_Open = 0;

/* 提示信息 */
char Message[BUF_LEN];

/* 进程读取的信息的指针 */
char *Message_Ptr;

/* 当进程试图打开设备文件时调用该函数*/
static int device_open(struct inode *inode, struct file *file){
    static int counter = 0;

    #ifdef DEBUG
    printk ("device_open(%p,%p)\n", inode, file);
    #endif

    /* 两个进程不得同时对同一设备操作.*/
    if (Device_Open)
        return -EBUSY;

    Device_Open++;

    /* 初始化信息. */
    sprintf(Message,"If I told you once, I told you %d times - Hello, world!\n",counter++);
    /* 仅在输出信息的最大长度大于 BUF_LEN, (这儿是80)使用sprintf.
     * 注意不得超过缓冲区长度,尤其是在内核中!! */
    Message_Ptr = Message;
    /* 当文件打开时确信这个模块存在.*/
    MOD_INC_USE_COUNT;
    return SUCCESS;
}

/* 当一个进程要关闭这个设备时,该函数被调用,这个调用不允许失败.*/
static int device_release(struct inode *inode, struct file *file){
    #ifdef DEBUG
    printk ("device_release(%p,%p)\n", inode, file);
    #endif
    Device_Open --;
    /* 设备文件使用计数器减1*/
    MOD_DEC_USE_COUNT;
    return 0;
}

/* 当一进程要从已打开的设备文件读数据时该函数被调用.*/
static ssize_t device_read(struct file *file,
     char *buffer,   /* 获得填充数据的缓冲区*/
     size_t length,loff_t *fops)     /*缓冲区中的数据长度(绝对不能越界!) */
     {
            /* 已写入到缓冲区buffer中的确切字节数*/
            int bytes_read = 0;
            char *ptr;
            int len=0;
            #ifdef DEBUG
            printk("device_read(%p,%p,%p,%d)\n",inode, file, buffer, length);
            #endif

            /* 如果已位于信息尾部,返回0*/
            if (*Message_Ptr == 0)
                return 0;
            ptr=Message_Ptr;
            len=0 ;
            /*取字符串的长度*/
            while (*(ptr++))
                len++;
            if (length<len)
                len=length;
            copy_to_user(buffer,Message_Ptr,len);
            /* 准确地把数据送到缓冲区buffer*/

            bytes_read=len;

            #ifdef DEBUG
            printk ("Read %d bytes, %d left\n",bytes_read, length);
            #endif

            /* 返回读取的实际字节数据 */
            return bytes_read;
 }

/* 当进程向这设备写(目前不支持)时调用本函数*/
static ssize_t device_write(struct file *file,const char *buffer,size_t length,loff_t *fops){
        #ifdef DEBUG
        printk ("device_write(%p,%p,%s,%d)",inode, file, buffer, length);
        #endif
        int len ;
        if (length>80)
            len=80;
        else
            len=length;
        copy_from_user(Message_Ptr,buffer,len);
        return -EINVAL;
}

/* 模块定义 ********************** */

/* 设备的主设备号*/
static int Major;

/* 当进程要对创建的设备进行某些操作时，这个结构存放了要调用的函数的入口
* 这个结构有系统设备表的指针指向。NULL 表示未实现该功能。*/

struct file_operations Fops = {
        read:  	device_read,
        write:	device_write,
        open:	device_open,
        release:device_release  /* a.k.a. close */
};

/* 初始化模块--注册字符设备 */
int init_module()
{
    /* 注册字符设备(至少一次) */
    Major = register_chrdev(0,DEVICE_NAME,&Fops);
    /* 负值意味出错 */
    if (Major < 0) {
        printk ("Sorry, registering the character device failed with %d\n",Major);
        return Major;
    }
    printk ("Registeration is a success. The major device number is %d.\n",Major);
    printk ("If you want to talk to the device driver, you'll have to\n");
    printk ("create a device file. We suggest you use:\n");
    printk ("mknod <name> c %d 0\n", Major);
    return 0;
}

/* 清除模块 --从 /proc 中注销合适的文件*/
void cleanup_module()
{
    int ret;
    /* 注销设备 */
    ret = unregister_chrdev(Major, DEVICE_NAME);
    /* 如果出错，报告错误*/
    if (ret < 0)
        printk("Error in module_unregister_chrdev: %d\n", ret);
}
/*在Redhat7.3以下版本请将其注释掉*/
MODULE_LICENSE("GPL");
