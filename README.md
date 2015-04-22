Salticidae plugins
==================
This repo contains plugins for the [Salticidae](https://github.com/rabits/salticidae) project

Video
-----

Video plugins can provide new video source

### Dummy
Displays test video table with current datetime. URL format is "dummy:///{width}x{height}x{fps}" - by default is 640x480x30.

### Raw
Can work with USB UVC & V4L2 devices under Linux & Android

#### Febon 100 CVBS UVC MJPEG (recommended)
This is small usb dongle [site](http://febon.blogspot.com/2012/02/1.html).
Have small power consumption (2 can be powered by Note 4), supports mjpeg (low bandwith usage), don't require specific driver (used UVC, built-in android kernel).

```
$ v4l2-ctl --all -d /dev/video1
Driver Info (not using libv4l2):
    Driver name   : uvcvideo
    Card type     : UVC Camera (046d:0900)
    Bus info      : usb-0000:00:12.2-1
    Driver version: 3.13.11
    Capabilities  : 0x84000001
        Video Capture
        Streaming
        Device Capabilities
    Device Caps   : 0x04000001
        Video Capture
        Streaming
Priority: 2
Video input : 0 (Camera 1: ok)
Format Video Capture:
    Width/Height  : 640/480
    Pixel Format  : 'MJPG'
    Field         : None
    Bytes per Line: 0
    Size Image    : 655360
    Colorspace    : Unknown (00000000)
Crop Capability Video Capture:
    Bounds      : Left 0, Top 0, Width 640, Height 480
    Default     : Left 0, Top 0, Width 640, Height 480
    Pixel Aspect: 1/1
Streaming Parameters Video Capture:
    Capabilities     : timeperframe
    Frames per second: 30.000 (30/1)
    Read buffers     : 0
                     brightness (int)    : min=0 max=255 step=1 default=128 value=128
                       contrast (int)    : min=0 max=31 step=1 default=16 value=16
                     saturation (int)    : min=0 max=31 step=1 default=16 value=16
                            hue (int)    : min=-180 max=180 step=1 default=0 value=0
```

#### EasyCAP Fushicai USBTV007
To use this hardware on your android smartphone, you need to build usbtv driver and replace your kernel.
Clone [Kernel N9005](https://github.com/rabits/kernel_n9005) repo and follow instructions in README.

Warning: supports only YUYV and totally uses usb 2.0 bandwith.

```
$ v4l2-ctl --all -d /dev/video0
Driver Info (not using libv4l2):
    Driver name   : usbtv
    Card type     : usbtv
    Bus info      : usb-0000:00:10.0-1
    Driver version: 3.13.6
    Capabilities  : 0x85000001
        Video Capture
        Read/Write
        Streaming
        Device Capabilities
    Device Caps   : 0x05000001
        Video Capture
        Read/Write
        Streaming
Priority: 2
Video input : 0 (Composite: ok)
Video Standard = 0x0000f900
    PAL-M/60
    NTSC-M/M-JP/443/M-KR
Format Video Capture:
    Width/Height  : 720/480
    Pixel Format  : 'YUYV'
    Field         : Interlaced
    Bytes per Line: 1440
    Size Image    : 691200
    Colorspace    : Broadcast NTSC/PAL (SMPTE170M/ITU601)
Streaming Parameters Video Capture:
    Frames per second: 29.970 (30000/1001)
    Read buffers     : 2
```
