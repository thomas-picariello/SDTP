#include "videoencoder.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 5;
static const int THREAD_SLEEP_MS = 20;

VideoEncoder::VideoEncoder(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH)
{

}

VideoEncoder::~VideoEncoder()
{
    stop();
}

void VideoEncoder::stop()
{
    m_stopped = true;
}


void VideoEncoder::addFrameToProcessingQueue(QVideoFrame frame)
{
    if (m_queue.length() < m_queueMaxLength) {
        QVideoFrame threadCopy;
        threadCopy = frame;
        m_queue.enqueue(threadCopy);
    } else {

        emit queueFull();
    }
}

void VideoEncoder::run()
{

    while (!m_stopped)
    {

        if (!m_queue.isEmpty())
        {
                // Processing here :
            //qDebug()<<"processing... : "<<m_queue.dequeue();
            m_queue.dequeue();
            qDebug()<<"processing";


            emit frameProcessed("Hello World");

        }
        else
        {
            msleep(THREAD_SLEEP_MS);
        }
    }
    exit(0);
}
