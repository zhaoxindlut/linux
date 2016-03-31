#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/fcntl.h>
#include <linux/hdreg.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>   //request_queue
#include <linux/sched.h>
#include <linux/fs.h>       //定义了block_device_operations

#define MAXBUF 1024       //最大传输数据段
#define BLK_MAJOR 253     //主设备号

char blk_dev_name[]="blk_dev";
static char flash[1024*16];

int major;
spinlock_t lock;
struct gendisk *gd;

/*块设备数据传输*/
static void blk_transfer(unsigned long sector, unsigned long nsect, char *buffer, int write)
{
  int read!=write;
  if(read)
  {
    memmove(buffer,flash+sector*512,nsect*512);  //内核中最好使用memmove函数进行内存数据的移动。memcpy为覆盖，memmove为移动。
  }
  else
  {
    memmove(flash+sector*512,buffer,nsect*512);
  }
}

/*块设备请求处理函数*/
static void blk_request_func(struct request_queue *q)
{
  struct request *req;
  while(req=elv_next_request(q)!=NULL)
  {
    if(!blk_fs_request(req))
    {
      end_request(req,0);
      continue;
    }
    blk_transfer(req->sector, req->current_nr_sectors, req->buffer, rq_data_dir(req));
    end_request(req,1);
  }
}

static int blk_ioctl(struct block_device *dev,fmode_t no,unsigned cmd, unsigned long arg)
{
  return -ENOTTY;
}

static int blk_open(struct block_device *dev, fmode_t no)
{
  printk("blk mount succeed\n");
  return 0;
}

static int blk_release(struct gendisk *gd, fmode_t no)
{
  printk("blk umount succeed\n");
  return 0;
}

struct block_device_operations blk_ops=
{
  .owner = THIS_MODULE,
  .open = blk_open,
  .release = blk_release,
  .ioctl = blk_ioctl,
};

static int __init slk_init(void)
{
  if(!register_blkdev(BLK_MAJOR, blk_dev_name))
  {
    major= BLK_MAJOR;
    printk("Register blk dev succeed\n");
  }
  else
  {
    return -EBUSY;
  }
  gd= alloc_disk(1);   //分配一个gendisk
  spin_lock_init(&lock);  //初始化自旋锁
  gd->major =major;
  gd->first_minor=0;   //第一个次设备号
  gd->fops= &blk_ops;   //关联操作函数
  gd->queue= blk_init_queue(blk_request_func,&lock);  //初始化请求队列并关联到gendisk
  snprintf(gd->disk_name, 32,"blk%c",'a');
  blk_queue_hardsect_size(gd->queue,512);     //设置扇区大小为512
  set_capacity(gd,32);  //设置块大小为512*32=16K
  add_disk(gd);
  printk("gendisk init successfully!\n");
  return 0;
}

static void __exit blk_exit(void)
{
  blk_cleanup_queue(gd->queue);
  del_gendisk(gd);
  unregister_blkdev(BLK_MAJOR,blk_dev_name);
  printk("block modlue exit successfully!\n");
}

module_init(blk_init);
module_exit(blk_exit);

MODULE_AUTHOR("ZHAOXIN");
MODULE_LICENSE("GPL");
