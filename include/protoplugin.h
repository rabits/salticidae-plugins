#ifndef PROTOPLUGIN_H
#define PROTOPLUGIN_H

#include <QUrl>

#ifndef PLUGIN_VERSION
#  define PLUGIN_VERSION "undefined plugin version"
#endif
#ifndef PLUGIN_NAME
#  define PLUGIN_NAME "undefined plugin name"
#endif
#ifndef PLUGIN_TYPE
#  define PLUGIN_TYPE "undefined plugin type"
#endif

class ProtoPlugin
{
public:
    virtual ~ProtoPlugin() {}

    // Plugin version, name and type should be predefined in plugin project file
    virtual QString version() { return PLUGIN_VERSION; }
    virtual QString name()    { return PLUGIN_NAME; }
    virtual QString type()    { return PLUGIN_TYPE; }

    virtual ProtoPlugin* instance(QUrl url) = 0;
};

#define ProtoPlugin_iid "org.rabits.salticidae.plugins"

Q_DECLARE_INTERFACE(ProtoPlugin, ProtoPlugin_iid)

#endif // PROTOPLUGIN_H
