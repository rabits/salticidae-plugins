#include "rotate.h"

#include <QDebug>

Rotate::Rotate(QObject *parent)
    : ProtoTransform(parent)
    , _angle(0)
{
    qDebug() << "[Plugin Transform Rotate] Creating plugin";
}

Rotate::~Rotate()
{
    qDebug() << "[Plugin Transform Rotate] Destroying plugin";
}

QMap<QString, QVariant> Rotate::getConfig()
{
    QMap<QString, QVariant> defaults;
    defaults["angle"] = _angle;

    return defaults;
}

void Rotate::setConfig(QMap<QString, QVariant> settings)
{
    if( settings.contains("angle") )
        _angle = settings["angle"].toInt();
}

QImage Rotate::process(QImage image)
{
    QTransform transform;
    transform.rotate(_angle);

    return image.transformed(transform);
}

