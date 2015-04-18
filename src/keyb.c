#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#define PROG_NAME "c2h2_spi_kbd"
int fire_key(__u16);
int setup_uinputfd(const char *);
int close_uinputfd();
void write_uinput(__u16, __u16, __s32);

/*fd for uinput, we do need kernel to support uinput */
static int uinputfd = -1;

int main(int argc, char **argv){
    puts("Welcome to use SPI keyboard program v0.1!");

    uinputfd = setup_uinputfd(PROG_NAME);
    if(uinputfd != -1){
        fire_key(KEY_A);
        fire_key(KEY_B);
        //fire_key(KEY_ENTER);
        //fire_key(KEY_A);
    }else{
        puts("where is uinput ? do you have permission?\n");
    }

    close_uinputfd();
    while(1);
    //exit(0);
}

int fire_key(__u16 key){
    write_uinput(EV_KEY, key, 1);
    return 0;
}

void write_uinput(__u16 type, __u16 code, __s32 value){
    struct input_event ie;

    usleep(10000);

    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = value;

    if(write(uinputfd, &ie, sizeof(ie)) != sizeof(ie)) puts("ERR1");

    memset(&ie, 0, sizeof(ie));
    ie.type = EV_SYN;
    ie.code = SYN_REPORT;
    ie.value = 0;

    if(write(uinputfd, &ie, sizeof(ie)) != sizeof(ie)) puts("ERR2");
}

int close_uinputfd(){
    close(uinputfd);
    return 0;
}

int setup_uinputfd(const char *name){
    int fd;
    int key;
    struct uinput_user_dev dev;

    fd = open("/dev/input/uinput", O_RDWR);
    if (fd == -1) {
        fd = open("/dev/uinput", O_RDWR);
        if (fd == -1) {
            fd = open("/dev/misc/uinput", O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "could not open %s\n", "uinput");
                perror(NULL);
                return -1;
            }
        }
    }
    memset(&dev, 0, sizeof(dev));
    strncpy(dev.name, name, sizeof(dev.name));
    dev.name[sizeof(dev.name) - 1] = 0;
    if (write(fd, &dev, sizeof(dev)) != sizeof(dev) ||
        ioctl(fd, UI_SET_EVBIT, EV_KEY) != 0
        || ioctl(fd, UI_SET_EVBIT, EV_REP) != 0) {
        goto setup_error;
    }

    for (key = KEY_RESERVED; key <= KEY_UNKNOWN; key++) {
        if (ioctl(fd, UI_SET_KEYBIT, key) != 0) {
            goto setup_error;
        }
    }

    if (ioctl(fd, UI_DEV_CREATE) != 0) {
        goto setup_error;
    }
    return fd;

setup_error:
    fprintf(stderr, "could not setup %s\n", "uinput");
    perror(NULL);
    close(fd);
    return -1;
}

