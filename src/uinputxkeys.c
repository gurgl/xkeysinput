/*
 ============================================================================
 Name        : uinputxkeys.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>

int mains(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	int uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
	if (uinp_fd == -1)
	{

		printf("Unable to open /dev/uinput : %s\n", strerror(errno));
		return -1;
	}
	ioctl (uinp_fd, UI_SET_EVBIT, EV_KEY);
	ioctl (uinp_fd, UI_SET_EVBIT, EV_REP);
	sleep(1);



	struct input_event press_event;

	press_event.type = EV_KEY;
	press_event.code = KEY_A;
	press_event.value = 1;

	struct input_event syn_event;

	syn_event.type = EV_SYN;
	syn_event.code = SYN_REPORT;
	syn_event.value = 0;

	int i = 0;
	for(i = 0; i < 10; i++) {
		write (uinp_fd, &press_event, sizeof(press_event));
		//press_event.value = 0;
		write (uinp_fd, &syn_event, sizeof(syn_event));
		//press_event.value = 1;
		//puts("Loop");

	}

	return EXIT_SUCCESS;
}
