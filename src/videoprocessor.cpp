#include "videoprocessor.h"

// --- CONSTRUCTOR ---
VideoProcessor::VideoProcessor() {
    // you could copy data from constructor arguments to internal variables here.

    i=0;
}

// --- DECONSTRUCTOR ---
VideoProcessor::~VideoProcessor() {
    // free resources
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
    qDebug()<<i++;

    frame.unmap();
}

void VideoProcessor::decode(QByteArray array)
{

    emit decoded(QImage::fromData(array,"JPG"));
}
void VideoProcessor::stop()
{
}
