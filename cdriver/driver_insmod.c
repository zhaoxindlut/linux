#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <asm/uaccess.h>      //包含在内核空间和用户空间移动数据的函数

MODULE_AUTHOR("DLUT");
MODULE_LICENSE("GPL");

#define MEM_MALLOC_SIZE 4096
#define MEM_MAJOR 246
#define MEM_MINOR 1
char *mem_spvm;
struct cdev *mem_cdev;
struct class *mem_class;
static int __init driver_init_module(void);
static void __exit driver_exit_module(void);
module_init(driver_init_module);
module_exit(driver_exit_module);

//文件结构体中所用的函数，file_operations结构体中的函数不必完全实现
static int mem_open(struct inode *ind,struct file *filp);
static int mem_release(struct inode *ind, struct file *filp);
static ssize_t mem_read(struct file *filp,char __user *buf,size_t size,loff_t *fops);
static ssize_t mem_write(struct file *filp,const char __user *buf,size_t size,loff_t *fops);

//设备驱动文件结构体
struct file_operations mem_fops=
{
  .open= mem_open,
  .release= mem_release,
  .read= mem_read,
  .write= mem_write,
};

//实现模块初始化，卸载函数
int __init driver_init_module(void)
{
  int res;
  int devno=MKDEV(MEM_MAJOR,MEM_MINOR);
  mem_spvm=(char*)vmalloc(MEM_MALLOC_SIZE);
  if(mem_spvm==NULL)
    printk(KERN_INFO"vmalloc failed!\n");
  else
    printk(KERN_INFO"vmalloc successfully! addr=0x%x\n",(unsigned int)mem_spvm);

  /*分配新的设备对象*/
  mem_cdev=cdev_alloc();
  if(mem_cdev==NULL)
  {
    printk(KERN_INFO"cdev_alloc failed!\n");
    return 0;
  }

  /*设备对象初始化*/
  cdev_init(mem_cdev,&mem_fops);
  mem_cdev->owner=THIS_MODULE;
  mem_cdev->ops=&mem_fops;

  /*将设备对象添加到内核*/
  res=cdev_add(mem_cdev,devno,1);
  if(res)
  {
    cdev_del(mem_cdev);
    mem_cdev=NULL;
    printk(KERN_INFO"cdev add error\n");
  }
  else{
    printk(KERN_INFO"cdev add successfully\n");
  }

  /*建立系统设备类*/
  mem_class=class_create(THIS_MODULE,"myalloc");
  if(IS_ERR(mem_class)){
    printk(KERN_INFO"Failed in creating class.\n");
    return -1;
  }

  /*注册设备文件系统，并建立设备节点*/
  device_create(mem_class,NULL,MKDEV(MEM_MAJOR,MEM_MINOR),NULL,"myalloc");
  return 0;
}

void __exit driver_exit_module(void)
{
  if(mem_cdev!=NULL)
    cdev_del(mem_cdev);
    printk(KERN_INFO"cdev del ok\n");
    device_destroy(mem_class,MKDEV(MEM_MAJOR,MEM_MINOR));
    class_destroy(mem_class);
    if(mem_spvm!=NULL)
      vfree(mem_spvm);
      printk(KERN_INFO"vfree ok\n");
}

int mem_open(struct inode *ind,struct file *filp)
{
  printk(KERN_INFO"open vmalloc space\n");
  try_module_get(THIS_MODULE);            //模块引用计数加一
  return 0;
}

int mem_release(struct inode *ind,struct file *filp)
{
  printk(KERN_INFO"close vmalloc spacd\n");
  module_put(THIS_MODULE);               //模块引用计数减一
  return 0;
}

ssize_t mem_read(struct file *filp,char *buf,size_t size,loff_t *lofp)
{
  int res=-1;
  char *tmp;
  //struct inode *inodep;
  //inodep=filp->f_dentry->d_inode;
  tmp=mem_spvm;
  if(size>MEM_MALLOC_SIZE)
    size=MEM_MALLOC_SIZE;
  if(tmp!=NULL)
    res=copy_to_user(buf,tmp,size);   //将内核空间的内容复制到用户空间
  if(res==0)
    return size;
  else
    return 0;
}

ssize_t mem_write(struct file *filp,const char *buf,size_t size,loff_t *lofp)
{
  int res=-1;
  char *tmp;
  //struct inode *inodep;
  //inodep=filp->f_dentry->d_inode;
  tmp=mem_spvm;
  if(size>MEM_MALLOC_SIZE)
    size=MEM_MALLOC_SIZE;
  if(tmp!=NULL)
    res=copy_from_user(tmp,buf,size);
  if(res==0)
    return size;
  else
    return 0;
}
