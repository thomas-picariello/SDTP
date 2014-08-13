#include "videoprocessor.h"

VideoProcessor::VideoProcessor() {

    i=0;
}
VideoProcessor::~VideoProcessor() {

}
void VideoProcessor::process(QVideoFrame frame) {


    frame.map(QAbstractVideoBuffer::MapMode::ReadWrite);

    QImage image = QImage(frame.bits(),
                     frame.width(),
                     frame.height(),
                     QVideoFrame::imageFormatFromPixelFormat(
                          frame.pixelFormat())).copy(QRect(0,0,frame.width(),frame.height()));

    emit imgForDisplay(image);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG",20);
    emit finished(ba);
    //qDebug()<<i++;

    frame.unmap();
}

void VideoProcessor::decode(QByteArray array)
{

    emit decoded(QImage::fromData(array,"JPG"));
}
void VideoProcessor::stop()
{
}
