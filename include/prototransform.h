#ifndef PROTOTRANSFORM_H
#define PROTOTRANSFORM_H

#include <QImage>

#include "protoplugin.h"

class ProtoTransform
    : public QObject
    , public ProtoPlugin
{
    Q_OBJECT
    Q_INTERFACES(ProtoPlugin)

public:
    explicit ProtoTransform(QObject *parent = 0) : QObject(parent) {}
    virtual ~ProtoTransform() {}

    // Process transform of image data
    virtual QImage process(QImage image) = 0;

    // Create new instance of transformer
    virtual ProtoTransform* instance(QMap<QString, QVariant> &settings) = 0;
};

#define ProtoTransform_iid "org.rabits.salticidae.plugins.transform"

Q_DECLARE_INTERFACE(ProtoTransform, ProtoTransform_iid)

#endif // PROTOTRANSFORM_H
