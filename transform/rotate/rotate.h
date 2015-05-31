#ifndef ROTATE_H
#define ROTATE_H

#include "prototransform.h"

class Rotate
    : public ProtoTransform
{
public:
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoTransform_iid FILE "../../plugin.json")
    Q_INTERFACES(ProtoTransform)

public:
    Rotate(QObject *parent = 0);
    ~Rotate();

    Q_INVOKABLE QMap<QString, QVariant> getConfig();
    Q_INVOKABLE void setConfig(QMap<QString, QVariant> settings);

    QImage process(QImage image);

    ProtoTransform* instance() {
        return new Rotate();
    }

private:
    int _angle;
};

#endif // ROTATE_H
