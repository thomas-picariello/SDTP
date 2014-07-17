#include "processingthread.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 5;
static const int THREAD_SLEEP_MS = 50;

ProcessingThread::ProcessingThread(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH)
{
}

ProcessingThread::~ProcessingThread()
{
    stop();
}

void ProcessingThread::stop()
{
    m_stopped = true;
}

void ProcessingThread::addFrameToProcessingQueue(QVideoFrame *frame)
{
    if (m_queue.length() < m_queueMaxLength) {
        QVideoFrame threadCopy;
        threadCopy = *frame;
        m_queue.enqueue(threadCopy);
    } else {
        qDebug()<<"queue Full";
        emit queueFull();
    }
}

void ProcessingThread::run()
{
    // Process until stop() called
    qDebug()<<"processing thread starts";
    while (!m_stopped)
    {
        if (!m_queue.isEmpty())
        {
            QVideoFrame currentFrame = m_queue.dequeue();

            // EncoreStuff
            //Send Stuff
            //Decode Stuff
            //other stuff

            emit frameProcessed();
        }
        else
        {
            // No frames in queue, sleep for a short while
            msleep(THREAD_SLEEP_MS);
        }
    }
    qDebug() << "Processing thread ending";
    exit(0);
}
