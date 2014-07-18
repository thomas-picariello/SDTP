#include "videoencoder.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 5;
static const int THREAD_SLEEP_MS = 20;

VideoEncoder::VideoEncoder(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH){
}

VideoEncoder::~VideoEncoder(){
    stop();
}

void VideoEncoder::stop(){
    m_stopped = true;
}
void VideoEncoder::addFrameToProcessingQueue(QVideoFrame frame){
    if (m_queue.length() < m_queueMaxLength)m_queue.enqueue(frame);
    else emit queueFull();
}

void VideoEncoder::run(){
    while (!m_stopped)
    {
        if (!m_queue.isEmpty())
        {
            QVideoFrame framecopy = m_queue.dequeue();


            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            QImage(framecopy.bits(),
                 framecopy.width(),
                 framecopy.height(),
                 QVideoFrame::imageFormatFromPixelFormat(
                     framecopy.pixelFormat())
                   ).save(&buffer, "PNG"); // writes image into ba in PNG format





            emit frameProcessed(ba);

        }else msleep(THREAD_SLEEP_MS);
    }
    exit(0);
}
