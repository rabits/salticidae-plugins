# Common plugin project variables
PLUGIN_NAME   = $$basename(_PRO_FILE_PWD_)
type_dir      = $$dirname(_PRO_FILE_PWD_)
PLUGIN_TYPE   = $$basename(type_dir)

PLUGIN_VERSION_STR = '\\"$${PLUGIN_VERSION}\\"'
PLUGIN_NAME_STR    = '\\"$${PLUGIN_NAME}\\"'
PLUGIN_TYPE_STR    = '\\"$${PLUGIN_TYPE}\\"'
DEFINES      += PLUGIN_VERSION=\"$${PLUGIN_VERSION_STR}\"
DEFINES      += PLUGIN_NAME=\"$${PLUGIN_NAME_STR}\"
DEFINES      += PLUGIN_TYPE=\"$${PLUGIN_TYPE_STR}\"

# Build template & plugin config
TEMPLATE      = lib
CONFIG       += plugin

# Qt common libraries:
#  qml - required by qmlRegisterType
QT           += qml

# Name of plugin in format "libplugin-<type>-<name>"
TARGET        = $$qtLibraryTarget(plugin-$${PLUGIN_TYPE}-$${PLUGIN_NAME})

# Copy target to plugins directory
QMAKE_POST_LINK = cp -lf "lib$${TARGET}.so" ../../

# Interfaces include
INCLUDEPATH  += $$PWD/include

# Plugin interfaces
HEADERS += $$PWD/include/*.h
