#include "videoencoder.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 10;
static const int THREAD_SLEEP_MS = 40;

VideoEncoder::VideoEncoder(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH){
    img = new QImage;

}

VideoEncoder::~VideoEncoder(){
    stop();
    delete img;
}
void VideoEncoder::stop(){
    m_stopped = true;
}
void VideoEncoder::addFrameToProcessingQueue(QImage *frame){
    if (m_queue.length() < m_queueMaxLength){
        m_queue.enqueue(frame->copy(QRect(0,0,frame->width(),frame->height())));
    }
}

void VideoEncoder::run(){

    while (!m_stopped)
    {
        if(!m_queue.isEmpty()){

            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            m_queue.dequeue().save(&buffer, "JPG",10);

            emit frameProcessed(ba);
        }else {
            msleep(THREAD_SLEEP_MS);
        }
    }
    exit(0);
}
