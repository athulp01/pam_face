#ifndef CAPTURE_H
#define CAPTURE_H

#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <libv4lconvert.h>

extern int camera;

u_int8_t* convert_yuyv_to_rgb(u_int8_t*);
int set_format(int);
int request_buffer(int, int);
int query_buffer(int);
int queue_buffer(int);
int start_streaming(int);
int dequeue_buffer(int);
int stop_streaming(int);
u_int8_t* get_rgbframe();

#endif