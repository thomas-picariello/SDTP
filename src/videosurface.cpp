#include "videosurface.h"


const int FILLCOLOR = 0xFF0000;

VideoSurface::VideoSurface(QObject *parent) :
    QAbstractVideoSurface(parent)
{
    m_lastFrame = new QVideoFrame();

}

VideoSurface::~VideoSurface()
{
    stop();
}

QVideoFrame* VideoSurface::frame() const
{
    return m_lastFrame;
}
bool VideoSurface::isFormatSupported(const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const{
    Q_UNUSED(similar);

         const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
         const QSize size = format.frameSize();

         return imageFormat != QImage::Format_Invalid
                 && !size.isEmpty()
                 && format.handleType() == QAbstractVideoBuffer::NoHandle;
}
void VideoSurface::paint(QPainter *painter){

}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
}
bool VideoSurface::present(const QVideoFrame &frame)
{


    if(frame.isValid()){
        QVideoFrame videoFrame(frame);
        emit newFrame(&videoFrame);
        return true;
    }
    return false;



}
bool VideoSurface::start(const QVideoSurfaceFormat &format)
{
    if (isActive()) {
        stop();
    } else if (!format.frameSize().isEmpty()) {
        return QAbstractVideoSurface::start(format);
    }
    return false;

}

void VideoSurface::stop()
{
    delete m_lastFrame;
    QAbstractVideoSurface::stop();
}
