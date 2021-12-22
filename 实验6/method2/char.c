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

struct chardev_dev           //�����豸
{
    struct cdev cdev;
    unsigned char mem[MAX_SIZE];
};

struct chardev_dev *devp;
dev_t devno;                      //�豸��

//open����
static int chardev_open(struct inode *inode, struct file *filp){
    filp->private_data = devp;
    return 0;
}

//release����
static int chardev_release(struct inode *inode, struct file *filp){
    return 0;
}

//ioctl����
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
//read����
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

    if (copy_to_user(buf, dev->mem + p, count))                             //���ں˿ռ�õ�����
        ret = -EINVAL;
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %u byte(s) from %lu\n", count, p);
    }

    return ret;
                                }

                                //write����
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

    if (copy_from_user(dev->mem + p, buf, count))                //���û��ռ�õ�д�������
        ret = -EINVAL;
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "write %u byte(s) from %lu\n", count, p);
    }

    return ret;
                                        }

//llseek����
static loff_t chardev_llseek(struct file *filp, loff_t offset, int orig){
    loff_t ret = 0;

    switch (orig)                                     //�ж��ļ�ָ���λ�ã�ȷ���Ӻο�ʼ��д
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

                                                //�ַ��豸������������
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
//��ʼ������ں���
static int __init chardev_init(void){
    int ret;
    int err;

    ret = alloc_chrdev_region(&devno, 0, 1, "chardev");            //��̬�����豸��
    if (ret < 0)
        return ret;

    devp = kzalloc(sizeof(struct chardev_dev), GFP_KERNEL);         //�����ڴ�ռ�
    if (!devp)
    {
        ret = -ENOMEM;
        goto fail_malloc;
    }

    class = class_create(NULL, "chardev");                          //������ڵ�
    if (IS_ERR(class))
    {
        ret = PTR_ERR(class);
        printk(KERN_ERR "class create error %d\n", ret);
        goto fail_malloc;
    }

    class->devnode = chardev_devnode;                              //�����豸�ڵ�
    dev = device_create(class, NULL, devno, NULL, "chardev");
    if (IS_ERR(class))
    {
        ret = PTR_ERR(dev);
        printk(KERN_ERR "device create error %d\n", ret);
        goto bad_device_create;
    }


    cdev_init(&devp->cdev, &chardev_fops);                          //�󶨲��������Ľṹ��
    devp->cdev.owner = THIS_MODULE;
    err = cdev_add(&devp->cdev, devno, 1);                          //����cdev_add������cdev�ṹ��ע�ᵽ�ں�
    if (err)
        printk(KERN_NOTICE "Error %d adding chardev", err);

    return 0;

    bad_device_create:
            class_destroy(class);
            fail_malloc:
            unregister_chrdev_region(devno, 1);
            return ret;
}

//ע��
static void __exit chardev_exit(void){
    device_destroy(class, devno);
    class_destroy(class);
    cdev_del(&devp->cdev);
    kfree(devp);
    unregister_chrdev_region(devno, 1);
}

module_init(chardev_init);                                      //����ģ��
module_exit(chardev_exit);                                      //�˳�ģ��

MODULE_AUTHOR("hsy");
MODULE_LICENSE("GPL");

