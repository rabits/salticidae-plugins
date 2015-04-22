# Version
PLUGIN_VERSION = '1.1'
# Description
PLUGIN_DESCRIPTION = 'With this plugin you can use MJPG & YUYV encoded /dev/video* V4L2 devices'

# Include basic plugin defines
include(../../plugin.pri)

QT           += multimedia

# Additional headers & sources
HEADERS      += raw.h \
                v4l2.h
SOURCES      += raw.cpp \
                v4l2.c
