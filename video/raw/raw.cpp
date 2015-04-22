#include "raw.h"

#include <QDebug>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

Raw::Raw(QObject *parent)
    : ProtoVideo(parent)
    , _timer_id(0)
    , _device(NULL)
    , _pixel_format(0)
    , _frame(NULL)
    , _rgb_frame(NULL)
{
    qDebug() << "[Plugin Video Raw] Plugin initialized";
}

Raw::Raw(const QByteArray &device)
    : ProtoVideo(NULL)
    , _timer_id(0)
    , _device(NULL)
    , _pixel_format(0)
    , _frame(NULL)
    , _rgb_frame(NULL)
{
    qDebug() << "[Plugin Video Raw] Creating plugin";
    _device = v4l2_create_device(device.data());
    v4l2_open_device(_device);

    v4l2_format_t format;
    v4l2_get_format(_device, &format);

    _width = format.width;
    _height = format.height;
    _pixel_format = format.pixel_format;
    _frame = new unsigned char[v4l2_get_buffer_size(_device)];
    _rgb_frame = new unsigned char[_width * _height * 4]; // RGB32

    QString fourcc = "";
    fourcc += _pixel_format & 0xff;
    fourcc += (_pixel_format >> 8) & 0xff;
    fourcc += (_pixel_format >> 16) & 0xff;
    fourcc += (_pixel_format >> 24) & 0xff;
    qDebug() << "[Plugin Video Raw] Created video source width:" << _width << "height:" << _height << "FourCC:" << fourcc;
}

Raw::~Raw()
{
    qDebug() << "[Plugin Video Raw] Destroying plugin";
    stop();
    if( _device != NULL ) {
        v4l2_close_device(_device);
        v4l2_destroy_device(_device);
    }
    delete[] _frame;
    delete[] _rgb_frame;
}

QSize Raw::size()
{
    return QSize(_width, _height);
}

void Raw::start()
{
    if ( receivers(SIGNAL(present(QImage))) > 0 && _timer_id == 0 ) {
        qDebug() << "[Plugin Video Raw] Starting capture";
        v4l2_start_capture(_device);
        _timer_id = startTimer(0);
    }
}

void Raw::stop()
{
    if ( receivers(SIGNAL(present(QImage))) == 0 && _timer_id != 0 ) {
        qDebug() << "[Plugin Video Raw] Stopping capture";
        killTimer(_timer_id);
        _timer_id = 0;
        v4l2_stop_capture(_device);
    }
}

void Raw::timerEvent(QTimerEvent*)
{
    if( v4l2_grab_frame(_device, _pixel_format) == V4L2_STATUS_ERROR )
        return;

    v4l2_copy_frame(_device, _frame);

    QImage img;

    switch( _pixel_format ) {
        case V4L2_PIX_FMT_MJPEG:
            img = QImage::fromData(_frame, _device->buffers[0].length, "JPG");
            break;
        case V4L2_PIX_FMT_YUYV:
            ccvt_yuyv(_width, _height, _frame, _rgb_frame);
            img = QImage(_rgb_frame, _width, _height, QImage::Format_RGB32);
            break;
        default:
            qWarning("Unsupported pixel format");
            return;
    }

    emit present(img);
}

QList<QUrl> Raw::sources()
{
    QList<QUrl> out;

    QDir devDir("/dev");
    devDir.setFilter(QDir::System);

    QFileInfoList entries = devDir.entryInfoList(QStringList() << "video*");

    foreach (const QFileInfo &entryInfo, entries) {
        qDebug() << "[Plugin Video Raw] Checking video device:" << entryInfo.filePath();
        if( ! (entryInfo.isWritable() && entryInfo.isReadable()) ) {
            qDebug() << "[Plugin Video Raw]   trying to get access";
            QProcess::execute("su", QStringList() << "-c" << QString("chmod 666 ").append(entryInfo.filePath()));
        }

        v4l2_device_t* dev = v4l2_create_device(entryInfo.filePath().toLocal8Bit());
        if( V4L2_STATUS_OK == v4l2_open_device(dev) ) {
            qDebug() << "[Plugin Video Raw]   camera" << dev->name << "added";

            out.append(QUrl(entryInfo.filePath().toLocal8Bit()));
            out.last().setScheme("raw");
        }
        v4l2_close_device(dev);
        v4l2_destroy_device(dev);
    }

    return out;
}

bool Raw::isSupported(QUrl url)
{
    return this->sources().contains(url);
}
