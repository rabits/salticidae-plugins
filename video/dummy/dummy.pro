# Version
PLUGIN_VERSION = '0.1'
# Description
PLUGIN_DESCRIPTION = 'Dummy UEIT table for testing. Use "dummy:///640x480x30" address to set any output resolution and framerate'

# Include basic plugin defines
include(../../plugin.pri)

QT           += svg

# Additional headers & sources
HEADERS      += dummy.h
SOURCES      += dummy.cpp
