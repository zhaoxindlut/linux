#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/fcntl.h>

int main(void)
{
  int fd;
  int res;
  char buf[]="This is an example for character devices by zhaoxin!!!^-^";
  char buf_read[1024];
  if((fd=open("/dev/myalloc",O_RDWR))==-1)
  {
    printf("open driver_insmod WROND!!!\n");
    return 0;
  }
  else
    printf("open driver_insmod SUCCESS!\n");

  res=write(fd,buf,strlen(buf));
  if(res==0)
  {
    printf("Writing failed!!\n");
    return 0;
  }
  else
    printf("Buf:%s has been written to driver_insmod!\n",buf);
  if((res=read(fd,buf_read,strlen(buf)))==0)
  {
    printf("Read failed!\n");
    return 0;
  }
  else
    printf("Read from driver_insmod:%s",buf_read);
  close(fd);
  return 0;
}
