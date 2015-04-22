#ifndef PROTOVIDEO_H
#define PROTOVIDEO_H

#include <QList>
#include <QtQml>
#include <QImage>

#include "protoplugin.h"

class ProtoVideo
    : public QObject
    , public ProtoPlugin
{
    Q_OBJECT
    Q_INTERFACES(ProtoPlugin)

public:
    explicit ProtoVideo(QObject *parent = 0) : QObject(parent) {}
    virtual ~ProtoVideo() {}

    // Get list of found & supported sources or empty list
    virtual QList<QUrl> sources() = 0;

    // Get list of supported schemes
    virtual QStringList schemes() = 0;

    // Check url supporting by plugin
    virtual bool isSupported(QUrl url) = 0;

    // Get size of picture
    virtual QSize size() = 0;

    // Create new instance with url to use it as video source
    virtual ProtoVideo* instance(QUrl url) = 0;

signals:
    // Interface for the EyeDisplay object
    void present(QImage);

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
};

#define ProtoVideo_iid "org.rabits.salticidae.plugins.video"

Q_DECLARE_INTERFACE(ProtoVideo, ProtoVideo_iid)

#endif // PROTOVIDEO_H
