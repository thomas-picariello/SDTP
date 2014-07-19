#include "videoencoder.h"
#include <QDebug>

static const int QUEUE_MAX_LENGTH = 50;
static const int THREAD_SLEEP_MS = 20;
static const int IMAGE_WIDTH = 1280;
static const int IMAGE_HEIGHT = 720;

VideoEncoder::VideoEncoder(QObject *parent) :
    QThread(parent), m_stopped(false), m_queueMaxLength(QUEUE_MAX_LENGTH){
    img = new QImage;

}

VideoEncoder::~VideoEncoder(){
    stop();
}
void VideoEncoder::stop(){
    m_stopped = true;
}
void VideoEncoder::addFrameToProcessingQueue(QImage *frame){
    if (m_queue.length() < m_queueMaxLength){
        m_queue.enqueue(frame->copy(QRect(0,0,IMAGE_WIDTH,IMAGE_HEIGHT)));
        //qDebug()<<"queue filled";
    }
    else ;//qDebug()<<"queue Full";
}

void VideoEncoder::run(){


        int i = 0;


        while (!m_stopped)
        {


            if(!m_queue.isEmpty()){

                QByteArray ba;
                QBuffer buffer(&ba);
                buffer.open(QIODevice::WriteOnly);
               // QDataStream datastream(&ba,QIODevice::ReadWrite);

                //datastream << m_queue.dequeue();

                m_queue.dequeue().save(&buffer, "JPG",20);
                //qDebug()<<i++;
                emit frameProcessed(ba);




            }else {
                //qDebug()<<"going to sleep";
                msleep(THREAD_SLEEP_MS);
            }
        }



    exit(0);
}
