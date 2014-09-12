#ifndef PROTOEYE_H
#define PROTOEYE_H

#include <QList>
#include <QtQml>
#include <QAbstractVideoSurface>

#include "protoplugin.h"

class ProtoEye
    : public ProtoPlugin
{
public:
    virtual ~ProtoEye() {}

    // Register types in QML
    virtual void registerQmlType() = 0;

    // Get list of found & supported sources or empty list
    virtual QList<QUrl> sources() = 0;

    // Get list of supported schemes
    virtual QStringList schemes() = 0;

    // Interface for VideoOutput
    virtual QAbstractVideoSurface* videoSurface() const = 0;

    // Create new instance with url to use it as video source
    virtual ProtoEye* instance(QUrl url) = 0;
};

#define ProtoEye_iid "org.rabits.salticidae.plugins.eye"

Q_DECLARE_INTERFACE(ProtoEye, ProtoEye_iid)

#endif // PROTOEYE_H
