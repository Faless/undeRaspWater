#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ADDRESS 0x04
#define DEVICE "/dev/i2c-1"
#define LOCK_FILE "/var/lock/ardui2c.lock"

int lock() { return open(LOCK_FILE, O_RDWR | O_CREAT | O_EXCL); }

int main(int argc, char **argv) {

   int lockfd = lock();

   while (lockfd == -1) {
      // printf("Waiting for lock to become available\n");
      // fflush(stdout);
      usleep(500000);
      lockfd = lock();
   }

   int out = 0;
   float val;
   if (argc != 2) {
      printf("Missing parameter. Aborting");
      out = -1;
      goto end;
   }
   int file;
   if ((file = open(DEVICE, O_RDWR)) < 0) {

      printf("Fatal: unable to device (do you have privileges?)\n");
      out = -1;
      goto end;
   }
   if (ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
      printf("Fatal: unable to access i2c device  0x%x\n", ADDRESS);
      out = -1;
      goto end;
   }

   int r1, r2, i;
   r1 = write(file, argv[1], strlen(argv[1]));
   usleep(10000);
   char buf[20];
   r2 = read(file, buf, 20);
   for (i = 0; i < 20; i++) {
      if (buf[i] == 255) {
         buf[i] = '\0';
         break;
      }
   }
   if (i > 6) {
      printf("%s\n", buf);
   } else {
      val = atof(buf);
      printf("%f\n", val);
   }

end:
   close(file);
   close(lockfd);
   unlink(LOCK_FILE);
   return out;
}
