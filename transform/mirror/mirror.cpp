#include "mirror.h"

#include <QDebug>

Mirror::Mirror(QObject *parent)
    : ProtoTransform(parent)
    , _vertical(false)
    , _horizontal(false)
{
    qDebug() << "[Plugin Transform Mirror] Creating plugin";
}

Mirror::~Mirror()
{
    qDebug() << "[Plugin Transform Mirror] Destroying plugin";
}

QMap<QString, QVariant> Mirror::getConfig()
{
    QMap<QString, QVariant> defaults;
    defaults["vertical"] = _vertical;
    defaults["horizontal"] = _horizontal;

    return defaults;
}

void Mirror::setConfig(QMap<QString, QVariant> settings)
{
    if( settings.contains("vertical") )
        _vertical = settings["vertical"].toBool();
    if( settings.contains("horizontal") )
        _horizontal = settings["horizontal"].toBool();
}

QImage Mirror::process(QImage image)
{
    return image.mirrored(_horizontal, _vertical);
}

