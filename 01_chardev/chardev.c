#include "debug.h"
#include <linux/init.h>        //module_init() 和 module_exit()
#include <linux/sched.h>       //包含大部分内核API的定义
#include <linux/module.h>      //什么模块信息的宏，例如MODULE_AUTHOR(author)
#include <linux/version.h>     //包含内核版本信息的头文件
#include <linux/moduleparam.h> //创建模块参数的宏
#include <linux/types.h>       //内核模块的各种数据类型，例如dev_t
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>

#define DEVICE_NAME "chardev"
#define CLASS_NAME "chardev"
#define DEVICE_CNT 1
// 设备结构
struct chardev_t {
    struct cdev cdev;
    dev_t dev_id;          // 设备号
    struct class *class;   // 类
    struct device *device; // 设备
    u8 *data; // 设备数据
};
struct chardev_t chardev;

/*
open 时传递设备数据给 private_data 指针
*/
static int chardev_open(struct inode *inode, struct file *file)
{
    file->private_data = chardev.data;

    pr_debug("data size is %d Bytes\n", *(u32 *)file->private_data);
    return 0;
}

/*
读取指定长度的数据，返回值是读取成功的字节数
*/
static ssize_t chardev_read(struct file *file, char __user *buf, size_t cnt, loff_t *offt)
{

    ssize_t ret = 0;
    
    u32 data_size = *(u32 *)file->private_data;
    u8 *data = (file->private_data)+4;


    if(cnt<0)
        cnt = 0;
    else if(cnt>data_size)
        cnt = data_size;

    pr_debug("read %ld bytes from 0x%px\n", cnt, data);

    // copy_to_user 成功返回 0 ，失败会返回复制失败的字节数
    ret = copy_to_user(buf, data, cnt);
    if(ret)
        pr_err("copy_to_user return %ld\n",ret);

    return cnt-ret;
}

/*
写入指定长度的数据，返回值是写入成功的字节数
*/
static ssize_t chardev_write(struct file *file, const char __user *buf, size_t cnt, loff_t *offt)
{
    ssize_t ret = 0;

    u32 data_size = *(u32 *)file->private_data;
    u8 *data = (file->private_data)+4;

    if(cnt<0)
        cnt = 0;
    else if(cnt>data_size)
        cnt = data_size;
    
    pr_debug("write %ld bytes to 0x%px\n", cnt, data);

    // copy_from_user 成功返回 0 ，失败会返回复制失败的字节数
    ret = copy_from_user(data, buf, cnt);
    if(ret)
        pr_err("copy_from_user return %ld\n",ret);
    pr_debug("0x%02x\n",*data);

    return cnt-ret;
}


static int chardev_release(struct inode *inode, struct file *file)
{
    file->private_data = NULL;

    pr_debug("chardev_close\n");
    return 0;
}

// 定义操作函数
static struct file_operations chardev_fops = {
    .owner = THIS_MODULE,

    .open = chardev_open,
    .read = chardev_read,
    .write = chardev_write,
    .release = chardev_release,
};

/*
创建设备节点时会调用这个回调函数，设置设备节点的权限
*/
static char *my_class_devnode(struct device *dev, umode_t *mode)
{
    if (mode != NULL)
        *mode = 0666;
    return NULL;
}

static int __init chardev_init(void)
{
    // 1. 申请设备号
    alloc_chrdev_region(&chardev.dev_id, 0, DEVICE_CNT, DEVICE_NAME); /* 申请设备号 */
    pr_info("major=%d,minor=%d\n", MAJOR(chardev.dev_id), MINOR(chardev.dev_id));

    // 2. 初始化 cdev 结构体
    chardev.cdev.owner = THIS_MODULE;
    cdev_init(&chardev.cdev, &chardev_fops);

    // 3. 向内核添加一个 cdev
    cdev_add(&chardev.cdev, chardev.dev_id, DEVICE_CNT);

    // 4. 创建类
    chardev.class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(chardev.class))
    {
        return PTR_ERR(chardev.class);
    }
    chardev.class->devnode = my_class_devnode;

    // 5. 创建设备
    chardev.device = device_create(chardev.class, NULL, chardev.dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chardev.device))
    {
        return PTR_ERR(chardev.device);
    }

    // 6. 初始化设备私有数据
    #define SIZE 0x100
    chardev.data = kzalloc(SIZE,GFP_KERNEL);
    if(chardev.data == NULL)
    {
        pr_err("kzalloc faild\n");
        return -ENOMEM;
    }
    memset(chardev.data, 0, SIZE);
    *(u32 *)chardev.data = SIZE-4; // data 的前四个字节用于记录后面的数据长度

    pr_info("driver init\n");
    return 0;
}

static void __exit chardev_exit(void)
{
    kfree(chardev.data);

    // 注销字符设备驱动
    cdev_del(&chardev.cdev);                           // 从内核删除 cdev
    unregister_chrdev_region(chardev.dev_id, DEVICE_CNT); // 注销设备号

    // 删除设备节点和类
    device_destroy(chardev.class, chardev.dev_id);
    class_destroy(chardev.class);

    pr_info("driver exit\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_AUTHOR("Bob<gexbob@gmail.com>");
MODULE_DESCRIPTION("Linux character device driver demo");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
