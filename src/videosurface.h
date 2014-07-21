#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>
#include <QVideoSurfaceFormat>
#include <QDebug>



class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit VideoSurface(QObject *parent = 0);
    virtual ~VideoSurface();
    virtual void stop();
signals:
    void newFrame(QVideoFrame);
private:
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool present(const QVideoFrame &frame);

    QImage::Format imageFormat;
};

#endif // VIDEOSURFACE_H
