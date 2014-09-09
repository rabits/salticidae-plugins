#! [0]
TEMPLATE      = lib
CONFIG       += plugin
QT           += multimedia qml
INCLUDEPATH  += ../../include
HEADERS      += raw.h \
                v4l2.h
SOURCES      += raw.cpp \
                v4l2.c
TARGET        = $$qtLibraryTarget(raw)
