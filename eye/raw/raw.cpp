#include "raw.h"

#include <QDebug>

#define SAT(c) \
    if (c & (~255)) { if (c < 0) c = 0; else c = 255; }

#define LIMIT(x)  ( (x) > 0xffff ? 0xff : ( (x) <= 0xff ? 0 : ( (x) >> 8 ) ) )

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

Raw::Raw(QObject *parent)
    : ProtoEye(parent)
    , _timer_id(0)
    , _device(NULL)
    , _frame(NULL)
    , _rgb_frame(NULL)
{
    qDebug() << "[Plugin Eye Raw] Created without device name";
}

Raw::Raw(const QByteArray &device)
    : ProtoEye(NULL)
    , _timer_id(0)
    , _device(NULL)
    , _frame(NULL)
    , _rgb_frame(NULL)
{
    _device = v4l2_create_device(device.data());
    v4l2_open_device(_device);

    v4l2_format_t _format;
    v4l2_get_format(_device, &_format);

    _width = _format.width;
    _height = _format.height;
    _frame = new unsigned char[v4l2_get_buffer_size(_device)];
    _rgb_frame = new unsigned char[_width * _height * 4]; // RGB32

    QString fourcc = "";
    fourcc += _format.pixel_format & 0xff;
    fourcc += (_format.pixel_format >> 8) & 0xff;
    fourcc += (_format.pixel_format >> 16) & 0xff;
    fourcc += (_format.pixel_format >> 24) & 0xff;
    qDebug() << "[Plugin Eye Raw] Created eye width:" << _width << "height:" << _height << "FourCC:" << fourcc;
}

Raw::~Raw()
{
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
    if ( receivers(SIGNAL(present(QImage*))) > 0 && _timer_id == 0 ) {
        qDebug() << "[Plugin Eye Raw] Starting capture";
        v4l2_start_capture(_device);
        _timer_id = startTimer(0);
    }
}

void Raw::stop()
{
    if ( receivers(SIGNAL(present(QImage*))) == 0 && _timer_id != 0 ) {
        qDebug() << "[Plugin Eye Raw] Stopping capture";
        killTimer(_timer_id);
        _timer_id = 0;
        v4l2_stop_capture(_device);
    }
}

void Raw::timerEvent(QTimerEvent*)
{
    v4l2_grab_frame(_device);
    v4l2_copy_frame(_device, _frame);
    ccvt_yuyv(_width, _height, _frame, _rgb_frame);

    _image = QImage(_rgb_frame, _width, _height, QImage::Format_RGB32);
    emit present(&_image);
}

void Raw::ccvt_yuyv(int width, int height, const unsigned char *src, unsigned char *dst)
{
    int line, col;
    int y, yy;
    int u, v;
    int vr, ug, vg, ub;
    int r, g, b;
    const unsigned char *py, *pu, *pv;

    py = src;
    pu = src + 1;
    pv = src + 3;

    y = *py;
    yy = y << 8;
    u = *pu - 128;
    ug = 88 * u;
    ub = 454 * u;
    v = *pv - 128;
    vg = 183 * v;
    vr = 359 * v;

    for (line = 0; line < height; line++) {
        for (col = 0; col < width; col++) {
            r = LIMIT(yy + vr);
            g = LIMIT(yy - ug - vg);
            b = LIMIT(yy + ub );
            *dst++ = b;
            *dst++ = g;
            *dst++ = r;
            *dst++ = 0;
            py += 2;
            y = *py;
            yy = y << 8;
            if ( (col & 1) == 1) {
                pu += 4; // skip yvy every second y
                pv += 4; // skip yuy every second y
            }
            u = *pu - 128;
            ug = 88 * u;
            ub = 454 * u;
            v = *pv - 128;
            vg = 183 * v;
            vr = 359 * v;
        } // ..for col
    } /* ..for line */
}

QList<QUrl> Raw::sources()
{
    QList<QUrl> out;

    QDir devDir("/dev");
    devDir.setFilter(QDir::System);

    QFileInfoList entries = devDir.entryInfoList(QStringList() << "video*");

    foreach (const QFileInfo &entryInfo, entries) {
        qDebug() << "[Plugin Eye Raw] Checking video device:" << entryInfo.filePath();
        if( ! (entryInfo.isWritable() && entryInfo.isReadable()) ) {
            qDebug() << "[Plugin Eye Raw]   trying to get access";
            QProcess::execute("su", QStringList() << "-c" << QString("chmod 666 ").append(entryInfo.filePath()));
        }
        int fd = ::open(entryInfo.filePath().toLatin1().constData(), O_RDWR );
        if( fd == -1 )
            continue;

        bool isCamera = false;

        v4l2_input input;
        memset(&input, 0, sizeof(input));
        for( ; ::ioctl(fd, VIDIOC_ENUMINPUT, &input) >= 0; ++input.index ) {
            if (input.type == V4L2_INPUT_TYPE_CAMERA || input.type == 0) {
                isCamera = ::ioctl(fd, VIDIOC_S_INPUT, input.index) != 0;
                break;
            }
        }

        if( isCamera ) {
            // find out its driver "name"
            QString name;
            struct v4l2_capability vcap;
            memset(&vcap, 0, sizeof(struct v4l2_capability));

            if (ioctl(fd, VIDIOC_QUERYCAP, &vcap) != 0)
                name = entryInfo.fileName();
            else
                name = QString((const char*)vcap.card);
            qDebug() << "[Plugin Eye Raw]   camera added";

            out.append(QUrl(entryInfo.filePath().toLocal8Bit()));
            out.last().setScheme("raw");
        }
        ::close(fd);
    }

    return out;
}

bool Raw::isSupported(QUrl url)
{
    return this->sources().contains(url);
}

