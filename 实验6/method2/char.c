#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/device.h>

#define MAX_SIZE    0x1024
#define MEM_CLEAR         0x1

static struct class *class;
static struct device *dev;
static int lock = 0;

struct chardev_dev           //定义设备
{
    struct cdev cdev;
    unsigned char mem[MAX_SIZE];
};

struct chardev_dev *devp;
dev_t devno;                      //设备号

//open函数
static int chardev_open(struct inode *inode, struct file *filp){
    filp->private_data = devp;
    return 0;
}

//release函数
static int chardev_release(struct inode *inode, struct file *filp){
    return 0;
}

//ioctl函数
static long chardev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
    struct chardev_dev *dev = filp->private_data;
    switch (cmd){
        case MEM_CLEAR:
            memset(dev->mem, 0, MAX_SIZE);
            printk(KERN_INFO "chardev is set to zero\n");
            break;
            default:
                return -EINVAL;
    }
    return 0;
}
//read函数
static ssize_t chardev_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos){

    if(lock = 1){
        lock = 0;
    }

    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chardev_dev *dev = filp->private_data;

    if (p >= MAX_SIZE)
        return -EINVAL;

    if (count > MAX_SIZE - p)
        count = MAX_SIZE - p;

    if (copy_to_user(buf, dev->mem + p, count))                             //从内核空间得到数据
        ret = -EINVAL;
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %u byte(s) from %lu\n", count, p);
    }

    return ret;
                                }

                                //write函数
static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t size,loff_t *ppos){
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chardev_dev *dev = filp->private_data;

    if (p >= MAX_SIZE){
        chardev_ioctl(filp,1,1);
        return -EINVAL;
    }

    if (count > MAX_SIZE - p)
        count = MAX_SIZE - p;

    if (copy_from_user(dev->mem + p, buf, count))                //从用户空间得到写入的数据
        ret = -EINVAL;
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "write %u byte(s) from %lu\n", count, p);
    }

    return ret;
                                        }

//llseek函数
static loff_t chardev_llseek(struct file *filp, loff_t offset, int orig){
    loff_t ret = 0;

    switch (orig)                                     //判断文件指针的位置，确定从何开始读写
    {
        case 0:
            if (offset < 0)
            {
                ret = -EINVAL;
                break;
            }
            if (offset > MAX_SIZE)
            {
                ret = -EINVAL;
                break;
            }
            filp->f_pos = offset;
            ret = filp->f_pos;
            break;

            case 1:
                if ((filp->f_pos + offset) < 0 )
                {
                    ret = -EINVAL;
                    break;
                }

                if ((filp->f_pos + offset) > MAX_SIZE)
                {
                    ret = -EINVAL;
                    break;
                }
                filp->f_pos += offset;
                ret = filp->f_pos;
                break;

                default:
                    ret = -EINVAL;
                    break;
    }

    return ret;
                                                }

                                                //字符设备操作函数定义
static const struct file_operations chardev_fops ={
    .owner  =  THIS_MODULE,
    .llseek =  chardev_llseek,
    .read   =  chardev_read,
    .write  =  chardev_write,
    .unlocked_ioctl = chardev_ioctl,
    .open   =  chardev_open,
    .release = chardev_release,
    };

static char *chardev_devnode(struct device *dev, umode_t *mode){
    if (mode)
        *mode = 0666;

    return NULL;
}
//初始化，入口函数
static int __init chardev_init(void){
    int ret;
    int err;

    ret = alloc_chrdev_region(&devno, 0, 1, "chardev");            //动态申请设备号
    if (ret < 0)
        return ret;

    devp = kzalloc(sizeof(struct chardev_dev), GFP_KERNEL);         //分配内存空间
    if (!devp)
    {
        ret = -ENOMEM;
        goto fail_malloc;
    }

    class = class_create(NULL, "chardev");                          //创建类节点
    if (IS_ERR(class))
    {
        ret = PTR_ERR(class);
        printk(KERN_ERR "class create error %d\n", ret);
        goto fail_malloc;
    }

    class->devnode = chardev_devnode;                              //创建设备节点
    dev = device_create(class, NULL, devno, NULL, "chardev");
    if (IS_ERR(class))
    {
        ret = PTR_ERR(dev);
        printk(KERN_ERR "device create error %d\n", ret);
        goto bad_device_create;
    }


    cdev_init(&devp->cdev, &chardev_fops);                          //绑定操作函数的结构体
    devp->cdev.owner = THIS_MODULE;
    err = cdev_add(&devp->cdev, devno, 1);                          //调用cdev_add函数将cdev结构体注册到内核
    if (err)
        printk(KERN_NOTICE "Error %d adding chardev", err);

    return 0;

    bad_device_create:
            class_destroy(class);
            fail_malloc:
            unregister_chrdev_region(devno, 1);
            return ret;
}

//注销
static void __exit chardev_exit(void){
    device_destroy(class, devno);
    class_destroy(class);
    cdev_del(&devp->cdev);
    kfree(devp);
    unregister_chrdev_region(devno, 1);
}

module_init(chardev_init);                                      //加载模块
module_exit(chardev_exit);                                      //退出模块

MODULE_AUTHOR("hsy");
MODULE_LICENSE("GPL");

