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
       qDebug()<<"newVideoFrame";

        QVideoFrame copyframe = frame;

        copyframe.map(QAbstractVideoBuffer::MapMode::ReadOnly);

        emit newFrame(QImage(frame.bits(),
                             frame.width(),
                             frame.height(),
                             QVideoFrame::imageFormatFromPixelFormat(
                                  frame.pixelFormat())));
        copyframe.unmap();

        return true;
    }else if(frame.isValid())return true;
    else return false;


}

void VideoSurface::stop()
{

    QAbstractVideoSurface::stop();
}

