#ifndef RAW_H
#define RAW_H

#include <protovideo.h>

extern "C" {
    #include "v4l2.h"
}

class Raw
    : public ProtoVideo
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoVideo_iid FILE "../../plugin.json")
    Q_INTERFACES(ProtoVideo)

public:
    QList<QUrl> sources();
    QStringList schemes() { return QStringList("raw"); }

    bool isSupported(QUrl url);

    Raw(QObject *parent = 0); // Only for QML type
    Raw(const QByteArray& device);
    ~Raw();

    QSize size();

    ProtoVideo* instance(QUrl url) {
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

    v4l2_device_t* _device;

    int _width, _height;
    unsigned int _pixel_format;
    unsigned char *_frame;
    unsigned char *_rgb_frame;
};

#endif // RAW_H
