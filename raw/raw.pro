#! [0]
TEMPLATE      = lib
CONFIG       += plugin
QT           += multimedia qml
INCLUDEPATH  += ../..
HEADERS      += raw.h \
                v4l2.h
SOURCES      += raw.cpp \
                v4l2.c
TARGET        = $$qtLibraryTarget(raw)
