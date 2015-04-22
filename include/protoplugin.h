#ifndef PROTOPLUGIN_H
#define PROTOPLUGIN_H

#include <QUrl>
#include <QSettings>

#ifndef PLUGIN_VERSION
#  define PLUGIN_VERSION "undefined plugin version"
#endif
#ifndef PLUGIN_NAME
#  define PLUGIN_NAME "undefined plugin name"
#endif
#ifndef PLUGIN_TYPE
#  define PLUGIN_TYPE "undefined plugin type"
#endif
#ifndef PLUGIN_DESCRIPTION
#  define PLUGIN_DESCRIPTION "undefined plugin description"
#endif

class ProtoPlugin
{
public:
    virtual ~ProtoPlugin() {}

    virtual QString name()        { return PLUGIN_NAME; }
    virtual QString type()        { return PLUGIN_TYPE; }

    // Plugin version and description you can define in the plugin project file
    virtual QString version()     { return PLUGIN_VERSION; }
    virtual QString description() { return PLUGIN_DESCRIPTION; }
};

#define ProtoPlugin_iid "org.rabits.salticidae.plugins"

Q_DECLARE_INTERFACE(ProtoPlugin, ProtoPlugin_iid)

#endif // PROTOPLUGIN_H
