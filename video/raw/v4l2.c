#include "v4l2.h"
#include "dht_table.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define SAT(c) \
    if (c & (~255)) { if (c < 0) c = 0; else c = 255; }

#define LIMIT(x)  ( (x) > 0xffff ? 0xff : ( (x) <= 0xff ? 0 : ( (x) >> 8 ) ) )

static int xioctl(int fd, int request, void *arg) {
    int r;

    do {
        r = ioctl(fd, request, arg);
    } while (0 > r && EINTR == errno);

    return r;
}

v4l2_device_t* v4l2_create_device(const char* device_name) {
    v4l2_device_t* device = (v4l2_device_t*)malloc(sizeof(v4l2_device_t));

    strncpy(device->name, device_name, sizeof(device->name));

    return device;
}

void v4l2_destroy_device(v4l2_device_t* device) {
    free(device);
}

int v4l2_open_device(v4l2_device_t* device) {
    struct stat st;

    //identify device
    if (0 > stat(device->name, &st)) {
        fprintf(stderr, "[Plugin Video Raw] Cannot identify device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "[Plugin Video Raw] Unrecognized device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    //open device
    device->fd = open(device->name, O_RDWR | O_NONBLOCK, 0);

    if (0 > device->fd) {
        fprintf(stderr, "[Plugin Video Raw] Unable to open device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    struct v4l2_capability cap;

    //query capabilities
    if (0 > xioctl(device->fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "[Plugin Video Raw] ERROR: %s is no V4L2 device\n", device->name);
        } else {
            fprintf(stderr, "[Plugin Video Raw] ERROR: Unable to query capabilities on device: %s\n", device->name);
        }

        return V4L2_STATUS_ERROR;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: %s is no video capture device\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: %s does not support streaming\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    //init mmap
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (0 > xioctl(device->fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "[Plugin Video Raw] ERROR: %s does not support memory mapping\n", device->name);
        } else {
            fprintf(stderr, "[Plugin Video Raw] ERROR requesting buffers on device: %s\n", device->name);
        }

        return V4L2_STATUS_ERROR;
    }

    if (req.count < 2) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: Insufficient buffer memory on %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    //allocate buffers
    device->buffers = (v4l2_buffer_t*)calloc(req.count, sizeof(v4l2_buffer_t));

    if (!device->buffers) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: Out of memory");
        return V4L2_STATUS_ERROR;
    }

    //map buffers
    unsigned int i;
    for (i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (0 > xioctl(device->fd, VIDIOC_QUERYBUF, &buf)) {
            fprintf(stderr, "[Plugin Video Raw] ERROR: Unable to query buffers on %s\n", device->name);
            return V4L2_STATUS_ERROR;
        }

        device->buffers[i].length = buf.length;
        device->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, device->fd, buf.m.offset);

        if (MAP_FAILED == device->buffers[i].start) {
            fprintf(stderr, "[Plugin Video Raw] ERROR: Unable to map buffers on %s\n", device->name);
            return V4L2_STATUS_ERROR;
        }
    }

    device->num_buffers = req.count;

    return V4L2_STATUS_OK;
}

int v4l2_close_device(v4l2_device_t *device) {
    unsigned int i;
    for (i = 0; i < device->num_buffers; i++) {
        if (0 > munmap(device->buffers[i].start, device->buffers[i].length)) {
            fprintf(stderr, "[Plugin Video Raw] Unable to unmap buffers on %s\n", device->name);
            return V4L2_STATUS_ERROR;
        }
    }

    if (0 > close(device->fd)) {
        fprintf(stderr, "[Plugin Video Raw] Unable to close device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    return V4L2_STATUS_OK;
}

size_t v4l2_get_buffer_size(v4l2_device_t* device) {
    if (device->num_buffers > 0) {
        return device->buffers[0].length;
    } else {
        return 0;
    }
}

int v4l2_set_format(v4l2_device_t* device, v4l2_format_t* format) {
    struct v4l2_format fmt;

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = format->width;
    fmt.fmt.pix.height = format->height;
    fmt.fmt.pix.pixelformat = format->pixel_format;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (0 > xioctl(device->fd, VIDIOC_S_FMT, &fmt)) {
        fprintf(stderr, "[Plugin Video Raw] Could not set format on device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    } else {
        return V4L2_STATUS_OK;
    }
}

int v4l2_get_format(v4l2_device_t* device, v4l2_format_t* format) {
    struct v4l2_format fmt;

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 > xioctl(device->fd, VIDIOC_G_FMT, &fmt)) {
        fprintf(stderr, "[Plugin Video Raw] Could not get format on device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    format->width = fmt.fmt.pix.width;
    format->height = fmt.fmt.pix.height;
    format->pixel_format = fmt.fmt.pix.pixelformat;

    return V4L2_STATUS_OK;
}

int v4l2_start_capture(v4l2_device_t *device) {
    //allocate data buffer
    device->data = (unsigned char*)malloc(device->buffers[0].length);

    //queue buffers
    unsigned int i;
    for (i = 0; i < device->num_buffers; i++) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (0 > xioctl(device->fd, VIDIOC_QBUF, &buf)) {
            fprintf(stderr, "[Plugin Video Raw] Unable to queue buffers on device: %s\n", device->name);
            return V4L2_STATUS_ERROR;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    //turn on stream
    if (0 > xioctl(device->fd, VIDIOC_STREAMON, &type)) {
        fprintf(stderr, "[Plugin Video Raw] Unable to turn on stream on device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    return V4L2_STATUS_OK;
}

int v4l2_stop_capture(v4l2_device_t *device) {
    //free data buffer
    free(device->data);

    //turn off stream
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 > xioctl(device->fd, VIDIOC_STREAMOFF, &type)) {
        fprintf(stderr, "[Plugin Video Raw] Unable to turn off stream on device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    // We need to wait after stopping device
    sleep(1);

    return V4L2_STATUS_OK;
}

int v4l2_grab_frame(v4l2_device_t *device, unsigned int format) {
    struct v4l2_buffer frame_buffer;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(device->fd, &fds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int i = select(device->fd+1, &fds, NULL, NULL, &timeout);

    if( 0 > i ) {
        if( EINTR != errno )
            fprintf(stderr, "[Plugin Video Raw] ERROR: select failed: %s\n", strerror(errno));
        return V4L2_STATUS_ERROR;
    } else if( 0 == i ) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: select timeout: %s\n", strerror(errno));
        return V4L2_STATUS_ERROR;
    } else if( !FD_ISSET(device->fd, &fds) )
        return V4L2_STATUS_OK;

    //dequeue buffer
    CLEAR(frame_buffer);

    frame_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frame_buffer.memory = V4L2_MEMORY_MMAP;

    if (0 > xioctl(device->fd, VIDIOC_DQBUF, &frame_buffer)) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: VIDIOC_DQBUF\n");
        return V4L2_STATUS_ERROR;
    }

    switch( format ) {
        case V4L2_PIX_FMT_MJPEG:
            memcpy_mjpg(device->data, device->buffers[frame_buffer.index].start, frame_buffer.bytesused);
            break;
        case V4L2_PIX_FMT_YUYV:
            memcpy(device->data, device->buffers[frame_buffer.index].start, frame_buffer.bytesused);
            break;
        default:
            fprintf(stderr, "[Plugin Video Raw] ERROR: Unsupported format\n");
            return V4L2_STATUS_ERROR;
    }

    //requeue buffer
    if (0 > xioctl(device->fd, VIDIOC_QBUF, &frame_buffer)) {
        fprintf(stderr, "[Plugin Video Raw] ERROR: Could not requeue buffer on device: %s\n", device->name);
        return V4L2_STATUS_ERROR;
    }

    return V4L2_STATUS_OK;
}

void v4l2_copy_frame(v4l2_device_t *device, unsigned char* dest) {
    memcpy(dest, device->data, device->buffers[0].length);
}

int is_huffman(unsigned char *buf)
{
  unsigned char *ptbuf;
  int i = 0;
  ptbuf = buf;
  while (((ptbuf[0] << 8) | ptbuf[1]) != 0xffda) {
    if (i++ > 2048)
      return 0;
    if (((ptbuf[0] << 8) | ptbuf[1]) == 0xffc4)
      return 1;
    ptbuf++;
  }
  return 0;
}

int memcpy_mjpg(unsigned char *out, unsigned char *buf, int size)
{
  unsigned char *ptdeb, *ptlimit, *ptcur = buf;
  int sizein, pos=0;

  if (!is_huffman(buf)) {
    ptdeb = ptcur = buf;
    ptlimit = buf + size;
    while ((((ptcur[0] << 8) | ptcur[1]) != 0xffc0) && (ptcur < ptlimit))
      ptcur++;
    if (ptcur >= ptlimit)
        return pos;
    sizein = ptcur - ptdeb;

    memcpy(out+pos, buf, sizein); pos += sizein;
    memcpy(out+pos, dht_data, sizeof(dht_data)); pos += sizeof(dht_data);
    memcpy(out+pos, ptcur, size - sizein); pos += size-sizein;
  } else {
    memcpy(out+pos, ptcur, size); pos += size;
  }
  return pos;
}

void ccvt_yuyv(int width, int height, const unsigned char *src, unsigned char *dst)
{
    int line, col;
    int y, yy;
    int u, v;
    int vr, ug, vg, ub;
    int r, g, b;
    const unsigned char *py, *pu, *pv;

    py = src;
    pu = src + 1;
    pv = src + 3;

    y = *py;
    yy = y << 8;
    u = *pu - 128;
    ug = 88 * u;
    ub = 454 * u;
    v = *pv - 128;
    vg = 183 * v;
    vr = 359 * v;

    for (line = 0; line < height; line++) {
        for (col = 0; col < width; col++) {
            r = LIMIT(yy + vr);
            g = LIMIT(yy - ug - vg);
            b = LIMIT(yy + ub );
            *dst++ = b;
            *dst++ = g;
            *dst++ = r;
            *dst++ = 0;
            py += 2;
            y = *py;
            yy = y << 8;
            if ( (col & 1) == 1) {
                pu += 4; // skip yvy every second y
                pv += 4; // skip yuy every second y
            }
            u = *pu - 128;
            ug = 88 * u;
            ub = 454 * u;
            v = *pv - 128;
            vg = 183 * v;
            vr = 359 * v;
        } // ..for col
    } /* ..for line */
}
