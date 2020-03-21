#include "capture.h"

u_int8_t *buffer;
int camera;

u_int8_t* convert_yuyv_to_rgb(u_int8_t* yuyv_image) {
    u_int8_t* rgb_image = (u_int8_t*) malloc(sizeof(unsigned char)*320*240*3);
    int y, cr, cb;
    double r,g,b;
    for (int i = 0, j = 0; i < 320 * 240 * 3; i+=6,j+=4) {
        //first pixel
        y = yuyv_image[j];
        cb = yuyv_image[j+1];
        cr = yuyv_image[j+3];

        r = y + (1.4065 * (cr - 128));
        g = y - (0.3455 * (cb - 128)) - (0.7169 * (cr - 128));
        b = y + (1.7790 * (cb - 128));

        //This prevents colour distortions in your rgb image
        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i] = (unsigned char)r;
        rgb_image[i+1] = (unsigned char)g;
        rgb_image[i+2] = (unsigned char)b;

        //second pixel
        y = yuyv_image[j+2];
        cb = yuyv_image[j+1];
        cr = yuyv_image[j+3];

        r = y + (1.4065 * (cr - 128));
        g = y - (0.3455 * (cb - 128)) - (0.7169 * (cr - 128));
        b = y + (1.7790 * (cb - 128));

        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i+3] = (unsigned char)r;
        rgb_image[i+4] = (unsigned char)g;
        rgb_image[i+5] = (unsigned char)b;
    }
    return rgb_image;
}

int set_format(int fd) {
    struct v4l2_format format = {0};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = 320;
    format.fmt.pix.height = 240;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    int res = ioctl(fd, VIDIOC_S_FMT, &format);
    if(res == -1) {
        perror("Could not set format");
        exit(1);
    }
    return res;
}

int request_buffer(int fd, int count) {
    struct v4l2_requestbuffers req = {0};
    req.count = count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req))
    {
        perror("Requesting Buffer");
        exit(1);
    }
    return 0;
}

int query_buffer(int fd) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    int res = ioctl(fd, VIDIOC_QUERYBUF, &buf);
    if(res == -1) {
        perror("Could not query buffer");
        return 2;
    }
    buffer = (u_int8_t*)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    return buf.length;
}

int queue_buffer(int fd) {
    struct v4l2_buffer bufd = {0};
    bufd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufd.memory = V4L2_MEMORY_MMAP;
    bufd.index = 0;
    if(-1 == ioctl(fd, VIDIOC_QBUF, &bufd))
    {
        perror("Queue Buffer");
        return 1;
    }
    return bufd.bytesused;
}

int start_streaming(int fd) {
    unsigned int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1){
        perror("VIDIOC_STREAMON");
        exit(1);
    }
}

int dequeue_buffer(int fd) {
    struct v4l2_buffer bufd = {0};
    bufd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufd.memory = V4L2_MEMORY_MMAP;
    bufd.index = 0;
    if(-1 == ioctl(fd, VIDIOC_DQBUF, &bufd))
    {
        perror("Dequeue Buffer");
        return 1;
    }
}

int stop_streaming(int fd) {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1){
        perror("VIDIOC_STREAMOFF");
        exit(1);
    }
}


u_int8_t* get_rgbframe() {
    camera = open("/dev/video0", O_RDWR);
    if(camera == -1) {
        perror("Could not open device");
        exit(1);
    }
    set_format(camera);
    request_buffer(camera, 1);
    int size = query_buffer(camera);
    start_streaming(camera);
    queue_buffer(camera);
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(camera, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(camera+1, &fds, NULL, NULL, &tv);
    if(-1 == r){
        perror("Waiting for Frame");
        exit(1);
    }
    dequeue_buffer(camera);
    char *name = (char*)malloc(sizeof(char)*11);
    sprintf(name, "pic%d.img", 1);
    int file = open(name, O_WRONLY|O_CREAT, 0777);
    printf("%d\n", buffer);
    if( -1 == write(file, buffer, size)) {
        perror("write failed");
        exit(1);
    }
    close(file);
    return convert_yuyv_to_rgb(buffer);
}
