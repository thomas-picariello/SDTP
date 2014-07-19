#include "videoencoder.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 10;
static const int THREAD_SLEEP_MS = 15;

VideoEncoder::VideoEncoder(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH){

}

VideoEncoder::~VideoEncoder(){
    stop();
}

void VideoEncoder::stop(){
    m_stopped = true;
}
void VideoEncoder::addFrameToProcessingQueue(QImage *frame){

    if (m_queue.length() < m_queueMaxLength)m_queue.enqueue(frame->copy(QRect(0,0,frame->width(),frame->height())));
    else emit queueFull();
}

void VideoEncoder::run(){

    QByteArray ba;
    QDataStream datastream(&ba,QIODevice::ReadWrite);
    while (!m_stopped)
    {
        if (!m_queue.isEmpty())
        {

            ba.clear();
            datastream << m_queue.dequeue();

            emit frameProcessed(ba);

        }else msleep(THREAD_SLEEP_MS);
    }
    exit(0);
}
