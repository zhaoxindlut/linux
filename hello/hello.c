#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
static int __init hello_init(void);
static int __init hello_exit(void);

static int __init hello_init(void)
{
  printk("Hello,Wrold!\n");
  return 0;
}
static int __exit hello_exit(void)
{
  printk("Bye!\n");
  return 0;
}
MODULE_AUTHOR("ZHAOXIN");
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
