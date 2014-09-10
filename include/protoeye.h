#ifndef PROTOEYE_H
#define PROTOEYE_H

#include <QList>
#include <QtQml>
#include <QAbstractVideoSurface>

class ProtoEye
{
public:
    virtual ~ProtoEye() {}

    // Plugin version and name should be predefined in plugin project file
    QString version() { return PLUGIN_VERSION; }
    QString name()    { return PLUGIN_NAME; }

    virtual void registerQmlType() = 0;
    virtual QList<QByteArray> sources() = 0;
    virtual QAbstractVideoSurface* videoSurface() const = 0;
};

#define ProtoEye_iid "org.rabits.salticidae.plugins.eye"

Q_DECLARE_INTERFACE(ProtoEye, ProtoEye_iid)

#endif // PROTOEYE_H
