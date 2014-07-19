#include "viewfinderwrapper.h"
#include <QDebug>

ViewFinderWrapper::ViewFinderWrapper(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_camera(0),
    m_processor(0)
{

    // Connect surface to our slot
    connect(&m_surface,SIGNAL(newFrame(QImage*)),this,SLOT(onNewFrame(QImage*)));


}

ViewFinderWrapper::~ViewFinderWrapper()
{
    m_processor->deleteLater();
    stopViewFinder();
    stopCamera();
}
void ViewFinderWrapper::startCamera()
{
    m_processor = new VideoEncoder(this);
    connect(m_processor, SIGNAL(queueFull()), this, SLOT(onThreadCongested()));
    connect(m_processor, SIGNAL(frameProcessed(QByteArray)),this,SIGNAL(newFrameToSend(QByteArray)));

    m_processor->start();

    m_camera = new QCamera(this);

    QList<QByteArray> cameras = m_camera->availableDevices();

    m_camera = new QCamera(cameras.first());
    m_camera->setViewfinder(new QCameraViewfinder);

    if (m_camera) {
        m_camera->start();
        if(m_camera->state()==QCamera::ActiveState){
            m_cameraActive = true;
            startViewFinder();
        }
        else qDebug()<<"camera didn't start";
    }

}

void ViewFinderWrapper::stopCamera()
{
    Q_ASSERT(m_camera);

    // Stop processing thread
    m_processor->stop();
    m_processor->deleteLater();
    m_processor = 0;

    // Stop viewfinder
    stopViewFinder();

    // Stop camera
    m_cameraActive = false;
    m_camera->stop();
    delete m_camera;
    m_camera = 0;
}

void ViewFinderWrapper::startViewFinder()
{
    if (!m_viewFinderActive)
    {
        qDebug()<<"viewfinder is being started";
        m_camera->setViewfinder( &m_surface);
        m_viewFinderActive = true;
    }
    else qDebug()<<"viewfinder is already activ...";
}

void ViewFinderWrapper::stopViewFinder()
{
    if (m_viewFinderActive)
    {
        m_receivedFrameCounter = 0;
        m_viewFinderActive = false;
        m_camera->setViewfinder(&m_surface );
    }
}
void ViewFinderWrapper::onNewFrame(QImage *frame){



    emit newFrameAvaillable(frame);

    if (m_processor) {
        m_processor->addFrameToProcessingQueue(frame);
    }

}
