#ifndef PROTOEYE_H
#define PROTOEYE_H

#include <QList>
#include <QtQml>
#include <QImage>

#include "protoplugin.h"

class ProtoEye
    : public QObject
    , public ProtoPlugin
{
    Q_OBJECT
    Q_INTERFACES(ProtoPlugin)

public:
    explicit ProtoEye(QObject *parent = 0) : QObject(parent) {}
    virtual ~ProtoEye() {}

    // Get list of found & supported sources or empty list
    virtual QList<QUrl> sources() = 0;

    // Get list of supported schemes
    virtual QStringList schemes() = 0;

    // Check url supporting by plugin
    virtual bool isSupported(QUrl url) = 0;

    // Get size of picture
    virtual QSize size() = 0;

    // Create new instance with url to use it as video source
    virtual ProtoEye* instance(QUrl url) = 0;

signals:
    // Interface for the EyeDisplay object
    void present(QImage*);

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
};

#define ProtoEye_iid "org.rabits.salticidae.plugins.eye"

Q_DECLARE_INTERFACE(ProtoEye, ProtoEye_iid)

#endif // PROTOEYE_H
