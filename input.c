#include "global.h"
#include "input.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <assert.h>

static int tty_fd = -1, old_kbd_mode = -1;
static char kbd_eventfile[256] = "/dev/input/by-id/";

static void read_keyboard(InputState* state) {
	static int fd = -1;
	
	if (fd < 0) {
		fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
	}
	
	struct {
		struct timeval time;
		unsigned short type;
		unsigned short code;
		unsigned int value;
	} data;
	memset(&data, 0, sizeof(data));

	while (true) {
		int bytes = read(fd, &data, sizeof(data));
		if (bytes < (int) sizeof(data))
			break;
		
		if (data.type == 1) {
			//printf("%u %u\n", data.code, data.value);
			// data.value == 1 if pressed
			switch (data.code) {
			case 17:
				state->forward = data.value;
				break;
			case 31:
				state->back = data.value;
				break;
			case 30:
				state->left = data.value;
				break;
			case 32:
				state->right = data.value;
				break;
			case 57:
				state->up = data.value;
				break;
			case 42:
				state->down = data.value;
				break;
			case 16:
				state->turnleft = data.value;
				break;
			case 18:
				state->turnright = data.value;
				break;
			case 1:
				quit();
				break;
			}
		}
	}
}

static void read_mouse(InputState* state) {
	static int fd = -1;
	
	state->dx = state->dy = 0;
	
	if (fd < 0) {
		fd = open("/dev/input/mice", O_RDONLY | O_NONBLOCK);
	}
	
	struct { signed char buttons, dx, dy; } data;
	
	while (true) {
		data.buttons = 0;
		data.dx = 0;
		data.dy = 0;
		
		int bytes = read(fd, &data, sizeof(data));
		if (bytes < (int) sizeof(data))
			break;
		
		state->dx += data.dx;
		state->dy += data.dy;
	}
}

static void unlock_keyboard() {
    ioctl(tty_fd, KDSKBMODE, old_kbd_mode);
}

void init_input() {
	// find keyboard event file
	/*
	DIR *d;
	struct dirent *dir;
	d = opendir("/dev/input/by-id");
	assert(d);

	while ((dir = readdir(d)) != NULL) {
		if (strstr(dir->d_name, "event-kbd") != NULL) {
			strcat(kbd_eventfile, dir->d_name); // unsafe i know
			printf("%s\n", kbd_eventfile);
			break;
		}
	}
	closedir(d);
	*/

	// prevent keyboard from writing to tty
	tty_fd = open("/dev/tty", O_RDONLY);
	ioctl(tty_fd, KDGKBMODE, &old_kbd_mode);
	ioctl(tty_fd, KDSKBMODE, K_OFF);
	atexit(unlock_keyboard);
}

void read_input(InputState* state) {
	read_keyboard(state);
	read_mouse(state);
}
