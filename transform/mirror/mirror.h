#ifndef MIRROR_H
#define MIRROR_H

#include "prototransform.h"

class Mirror
    : public ProtoTransform
{
public:
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoTransform_iid FILE "../../plugin.json")
    Q_INTERFACES(ProtoTransform)

public:
    Mirror(QObject *parent = 0);
    ~Mirror();

    Q_INVOKABLE QMap<QString, QVariant> getConfig();
    Q_INVOKABLE void setConfig(QMap<QString, QVariant> settings);

    QImage process(QImage image);

    ProtoTransform* instance() {
        return new Mirror();
    }

private:
    bool _vertical;
    bool _horizontal;
};

#endif // MIRROR_H
