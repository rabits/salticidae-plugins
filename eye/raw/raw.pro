# Version
PLUGIN_VERSION = '1.0'

# Include basic plugin defines
include(../../plugin.pri)

QT           += multimedia

# Additional headers & sources
HEADERS      += raw.h \
                v4l2.h
SOURCES      += raw.cpp \
                v4l2.c
