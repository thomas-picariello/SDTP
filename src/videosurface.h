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
signals:
    void newFrame(QImage);

public:
    virtual void stop();

private:
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool present(const QVideoFrame &frame);
private:
    QWidget *widget;
     QImage::Format imageFormat;
     QRect targetRect;
     QSize imageSize;
     QRect sourceRect;
     QByteArray currentFrame;

};

#endif // VIDEOSURFACE_H
