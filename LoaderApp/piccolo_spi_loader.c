#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h> 
#include "../piccolo_spi_bootloader_io.h"

#define CHAR_DEVICE "/dev/piccolobl"

int main(int argc, char **argv)
{
   int fd= 0;
   char buf[200];
   if ((fd = open(CHAR_DEVICE, O_RDWR)) < -1)
   {
   
      printf("Failed to open char device");
      return 1;
   }
   /* Read 20 bytes */
   if (read(fd, buf, 40) > 0)
      printf("%s\n", buf);
      
      
   uint8_t new_firmware[10];
   
   int size = write(fd , new_firmware, sizeof(new_firmware));
   printf("sizeWritten = %d\n" , size);
   
   printf ("Calling IOCTL");
   ioctl(fd, START_UPLOAD); /* ioctl call to erase partition */
   //ioctl(fd, EEP_GET_SIZE, &size); /* ioctl call to get partition size */
   //ioctl(fd, ENABLE_DISABLE_LOADER, 1); /* ioctl call to rename partition */
   
   
   close(fd);
   
   return 0;
}