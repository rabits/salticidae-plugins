#ifndef DUMMY_H
#define DUMMY_H

#include <protovideo.h>

class Dummy
    : public ProtoVideo
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ProtoVideo_iid FILE "../../plugin.json")
    Q_INTERFACES(ProtoVideo)

public:
    QList<QUrl> sources();
    QStringList schemes() { return QStringList("dummy"); }

    bool isSupported(QUrl url);

    Dummy(QObject *parent = 0); // Only for QML type
    Dummy(const QString& size_fps);
    ~Dummy();

    QSize size();

    ProtoVideo* instance(QUrl url) {
        return new Dummy(url.path().mid(1));
    }

public slots:
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent*);

private:
    uint _timer_id;
    uint _width, _height;
    uint _framerate;

    QImage _base_image;
};

#endif // DUMMY_H
