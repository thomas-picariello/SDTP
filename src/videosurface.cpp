#include "videosurface.h"


const int FILLCOLOR = 0xFF0000;

VideoSurface::VideoSurface(QObject *parent) :
    QAbstractVideoSurface(parent)
{

}
VideoSurface::~VideoSurface()
{
    stop();
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
}
bool VideoSurface::present(const QVideoFrame &frame)
{
    if(frame.isValid())
    {
        QVideoFrame copyframe;
        copyframe  = frame;
        emit newFrame(copyframe);
        return true;
    }else return false;
}

void VideoSurface::stop()
{
    QAbstractVideoSurface::stop();
}
