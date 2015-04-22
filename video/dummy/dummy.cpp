#include "dummy.h"
#include "ueit_svg.h"

#include <QDebug>
#include <QSvgRenderer>
#include <QPainter>
#include <QDateTime>

QList<QUrl> Dummy::sources()
{
    QList<QUrl> out;
    out.append(QUrl("dummy:///640x480x30"));

    return out;
}

bool Dummy::isSupported(QUrl url)
{
    QStringList parts = url.path().mid(1).split('x');
    qDebug() << "[Plugin Video Dummy] Checking parameters" << parts;
    bool out = true;

    if( parts[0].length() > 0 && parts[0].toUInt(&out) < 320 )
        out = false;
    if( out ) {
        if( parts.length() > 1 && parts[1].toUInt(&out) < 240 )
            out = false;

        if( out ) {
            if( parts.length() > 2 && parts[2].toUInt(&out) < 1 )
                out = false;
        }
    }

    if( ! out )
        qDebug() << "[Plugin Video Dummy]   parameters failed";
    else
        qDebug() << "[Plugin Video Dummy]   parameters passed";

    return out;
}

Dummy::Dummy(QObject *parent)
    : ProtoVideo(parent)
    , _timer_id(0)
    , _width(0)
    , _height(0)
    , _framerate(0)
{
    qDebug() << "[Plugin Video Dummy] Plugin initialized";
}

Dummy::Dummy(const QString &size_fps)
    : ProtoVideo(NULL)
    , _timer_id(0)
    , _width(640)
    , _height(480)
    , _framerate(30)
{
    qDebug() << "[Plugin Video Dummy] Creating plugin";
    QStringList parts = size_fps.split('x');

    qDebug() << parts;
    if( parts[0].length() > 0 )
        _width = parts[0].toUInt();
    if( parts.length() > 1 )
        _height = parts[1].toUInt();
    if( parts.length() > 2 )
        _framerate = parts[2].toUInt();

    _base_image = QImage(_width, _height, QImage::Format_RGB32);
    _base_image.fill(0xffffff);

    QByteArray svg_data = QByteArray(ueit_svg_data);
    QSvgRenderer renderer(svg_data);
    QPainter painter(&_base_image);
    renderer.render(&painter);

    qDebug() << "[Plugin Video Dummy] Created video source width:" << _width << "height:" << _height;
}

Dummy::~Dummy()
{
    qDebug() << "[Plugin Video Dummy] Destroying plugin";
    stop();
}

QSize Dummy::size()
{
    return QSize(_width, _height);
}

void Dummy::start()
{
    if ( receivers(SIGNAL(present(QImage))) > 0 && _timer_id == 0 ) {
        qDebug() << "[Plugin Video Dummy] Starting table generation" << 1000 / _framerate;
        _timer_id = startTimer(1000 / _framerate);
    }
}

void Dummy::stop()
{
    if ( receivers(SIGNAL(present(QImage))) == 0 && _timer_id != 0 ) {
        qDebug() << "[Plugin Video Dummy] Stopping capture";
        killTimer(_timer_id);
        _timer_id = 0;
    }
}

void Dummy::timerEvent(QTimerEvent*)
{
    QImage img = _base_image;
    img.detach();

    QPainter painter(&img);
    painter.drawText(_width/2, _height/15, QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss.zzz"));

    emit present(img);
}
