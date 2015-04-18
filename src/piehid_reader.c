/***************************************************
 X-Keys Test Program

 The code may be used by anyone for any purpose,
 and can serve as a starting point for developing
 applications using the X-Keys libary.

 Alan Ott
 Signal 11 Software
***************************************************/

#include "piehid/PieHid32.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "uinput_sender.h"

void print_buf(char *data, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		printf("%02hhx ", data[i]);
		if ((i+1) % 8 == 0)
			printf("  ");
		if ((i+1) % 16 == 0)
			printf("\n");
	}
	printf("\n\n");

}
extern int uinputfd;
extern int fire_key(__u16);
extern int release_key(__u16);
extern int setup_uinputfd(const char *);
extern int close_uinputfd();

extern int setup_uinputfd(const char *name);
extern sender();
int main(int argc, char **argv){
	reader();
}

int reader() {
    puts("Welcome to use SPI keyboard program v0.1!");

    uinputfd = setup_uinputfd(PROG_NAME);
    if(uinputfd != -1){
//        fire_key(KEY_A);
//        fire_key(KEY_B);
    }else{
        puts("where is uinput ? do you have permission?\n");
    }

    //close_uinputfd();

	TEnumHIDInfo info[128];
	long count;
	int i;
	long handle = -1;

	unsigned res = EnumeratePIE(PI_VID, info, &count);

	for (i = 0; i < count; i++) {
		TEnumHIDInfo *dev = &info[i];
		printf("Found XKeys Device:\n");
		printf("\tPID: %04x\n", dev->PID);
		printf("\tUsage Page: %04x\n", dev->UP);
		printf("\tUsage:      %04x\n", dev->Usage);
		printf("\tVersion: %d\n\n", dev->Version);


		handle = dev->Handle;
		unsigned int res = SetupInterfaceEx(handle);
		if (res != 0) {
			printf("Unabe to open device. err: %d\n", res);
		}
		break;
	}

	if (handle < 0) {
		printf("Unable to open device\n");
		exit(1);
	}

	int left_down;
	int middle_down;
	int right_down;

	char data[80];
	while (1) {

		unsigned int res = 0;

		res  = ReadLast(handle, data);
		if (res == 0) {
			//printf("LAST: \n");
			//print_buf(data, 33);
			//printf("ENDLAST\n\n");
		}

		res = 0;


		while (res == 0) {
			res = BlockingReadData(handle, data, 20);
			if (res == 0) {


				//puts("input kom");
				//print_buf(data, 33);
				//printf("DATA %x", data[3]);
				if(data[3] & 0x02) {
					left_down = 1;
					fire_key(KEY_LEFTSHIFT);
					printf("Left down");
				} else {
					if(left_down == 1) {
						release_key(KEY_LEFTSHIFT);
						printf("Left up");
					}
					left_down = 0;
				}
				if(data[3] & 0x04) {
					printf("Middle down");
				}

				if(data[3] & 0x08) {
					printf("Right down");
				}

				//fire_key(KEY_A);
			}
			else if (res == PIE_HID_READ_INSUFFICIENT_DATA) {
				//printf(".");
				fflush(stdout);
			}
			else {
				printf("Error Reading\n");
			}
		}

		//printf("Sleeping\n");
		#if 1
		if (res != 0) {
			usleep(10*1000); // Sleep 10 milliseconds.
			//sleep(2); // 2 seconds
		}
		#endif

		//ClearBuffer(handle);

	}


	return 0;
}
