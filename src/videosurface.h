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

    bool isFormatSupported(const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const;
    void paint(QPainter *painter);

signals:
    void frameAvailable();
    void newFrame(QVideoFrame*);
public:
    QVideoFrame *frame() const;
    virtual void stop();
private:
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool present(const QVideoFrame &frame);
    virtual bool start(const QVideoSurfaceFormat &format);
private:
    QVideoFrame *m_lastFrame;

    QWidget *widget;
     QImage::Format imageFormat;
     QRect targetRect;
     QSize imageSize;
     QRect sourceRect;
     QVideoFrame currentFrame;
};

#endif // VIDEOSURFACE_H
