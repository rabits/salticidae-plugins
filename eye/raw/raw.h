#ifndef RAW_H
#define RAW_H

#include <protoeye.h>

extern "C" {
    #include "v4l2.h"
}

class Raw
    : public ProtoEye
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoEye_iid FILE "plugin.json")
    Q_INTERFACES(ProtoEye)

public:
    QList<QUrl> sources();
    QStringList schemes() { return QStringList("raw"); }

    bool isSupported(QUrl url);

    Raw(QObject *parent = 0); // Only for QML type
    Raw(const QByteArray& device);
    ~Raw();

    QSize size();

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

    QImage _image;

    v4l2_device_t* _device;

    int _width, _height;
    unsigned char *_frame;
    unsigned char *_rgb_frame;

    void ccvt_yuyv(int width, int height, const unsigned char *src, unsigned char *dst);
};

#endif // RAW_H
