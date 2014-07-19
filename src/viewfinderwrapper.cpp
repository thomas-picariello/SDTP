#include "viewfinderwrapper.h"
#include <QDebug>

ViewFinderWrapper::ViewFinderWrapper(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_camera(0), m_cameraStatus(QCamera::UnloadedStatus), m_cameraError(QCamera::NoError),
    m_cameraActive(false), m_viewFinderActive(false), m_receivedFrameCounter(0), m_processedFrameCounter(0),
    m_processor(0)
{




    // Set flags to get our QML-element to draw
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    // Connect surface to our slot
    connect(&m_surface,SIGNAL(newFrame(QImage*)),this,SLOT(onNewFrame(QImage*)));


}

ViewFinderWrapper::~ViewFinderWrapper()
{
    m_processor->deleteLater();
    stopViewFinder();
    stopCamera();
}

int ViewFinderWrapper::cameraStatus() const
{
    return m_cameraStatus;
}

int ViewFinderWrapper::cameraError() const
{
    return m_cameraError;
}

long ViewFinderWrapper::frameCount() const
{
    return m_receivedFrameCounter;
}

long ViewFinderWrapper::processedCount() const
{
    return m_processedFrameCounter;
}

bool ViewFinderWrapper::running() const
{
    return m_viewFinderActive;
}

void ViewFinderWrapper::startCamera()
{
    Q_ASSERT(!m_camera);
    Q_ASSERT(!m_processor);

    m_processor = new VideoEncoder(this);
    connect(m_processor, SIGNAL(queueFull()), this, SLOT(onThreadCongested()));
    connect(m_processor, SIGNAL(frameProcessed(QByteArray)),this,SIGNAL(newFrameToSend(QByteArray)));

    m_processor->start();



    m_camera = new QCamera(this);

    QList<QByteArray> cameras = m_camera->availableDevices();

    m_camera = new QCamera(cameras.first());
    m_camera->setViewfinder(new QCameraViewfinder);

    if (m_camera) {
        connect(m_camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(onStateChanged(QCamera::State)));
        connect(m_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(onStatusChanged(QCamera::Status)));
        connect(m_camera, SIGNAL(error(QCamera::Error)), this, SLOT(onCameraError(QCamera::Error)));
        m_camera->start();
        if(m_camera->state()==QCamera::ActiveState){
            m_cameraActive = true;
            qDebug()<<"Camera seems to have started correctly...";
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

    // Reset state
    reset();
}

void ViewFinderWrapper::reset()
{
    m_cameraStatus = QCamera::UnloadedStatus;
    m_cameraError = QCamera::NoError;
    m_receivedFrameCounter = 0;
    m_processedFrameCounter = 0;
    update();
}

void ViewFinderWrapper::startViewFinder()
{
    if (!m_viewFinderActive)
    {
        qDebug()<<"viewfinder is being started";
        m_camera->setViewfinder( &m_surface);
        m_viewFinderActive = true;
        emit runningChanged();
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
        emit runningChanged();
    }
}

void ViewFinderWrapper::onStateChanged(QCamera::State state)
{
}

void ViewFinderWrapper::onStatusChanged(QCamera::Status status)
{
    m_cameraStatus = status;
    if (m_cameraStatus == QCamera::ActiveStatus) {
        startViewFinder();
    }
    emit cameraStatusChanged(m_cameraStatus);
}

void ViewFinderWrapper::onCameraError(QCamera::Error error)
{
    m_cameraError = error;
    emit cameraErrorChanged(m_cameraError);
}
void ViewFinderWrapper::onNewFrame(QImage *frame){



    emit newFrameAvaillable(frame);

    if (m_processor) {
        m_processor->addFrameToProcessingQueue(frame);
    }

}

void ViewFinderWrapper::onThreadCongested()
{

    //qDebug()<<"onthread congested";

}
