#ifndef RAW_H
#define RAW_H

#include <protoeye.h>

#include <QVideoSurfaceFormat>

extern "C" {
    #include "v4l2.h"
}

class Raw
    : public QObject
    , public ProtoEye
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoEye_iid FILE "plugin.json")
    Q_INTERFACES(ProtoEye)

    Q_PROPERTY( QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface )

public:
    void registerQmlType() { qmlRegisterType<Raw>(ProtoEye_iid, 1, 0, PLUGIN_NAME ); }

    QList<QUrl> sources();
    QStringList schemes() { return QStringList("raw"); }

    Raw(QObject *parent = 0); // Only for QML type
    Raw(const QByteArray& device);
    ~Raw();

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface* surface);

    ProtoEye* instance(QUrl url) {
        return new Raw(url.path().toLocal8Bit());
    }

public slots:
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent*);

private:
    void closeSurface();

    int _timer_id;

    QAbstractVideoSurface* _vs;
    QVideoSurfaceFormat    _vsformat;

    v4l2_device_t* _device;

    int _width, _height;
    unsigned char *_frame;
    unsigned char *_rgb_frame;

    void ccvt_yuyv(int width, int height, const unsigned char *src, unsigned char *dst);
};

#endif // RAW_H
