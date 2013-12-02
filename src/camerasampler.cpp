#include "camerasampler.h"

/*Constructors*/
CameraSampler::CameraSampler(QObject *parent) : QObject(parent){
    mPixelOffset = 0;
    mImage = new QImage();
    mCamera = new QCamera();
    mCapture = new QCameraImageCapture(mCamera);



    connect(mCamera, SIGNAL(captureModeChanged(QCamera::CaptureModes)),
            this, SLOT(captureModeChanged(QCamera::CaptureModes)));
    connect(mCamera, SIGNAL(error(QCamera::Error)),
            this, SLOT(error(QCamera::Error)));
    connect(mCamera, SIGNAL(lockFailed()),
            this, SLOT(locked()));
    connect(mCamera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),
            this, SLOT(lockStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)));
    connect(mCamera, SIGNAL(locked()),
            this, SLOT(locked()));
    connect(mCamera, SIGNAL(stateChanged(QCamera::State)),
            this, SLOT(stateChanged(QCamera::State)));
    connect(mCamera, SIGNAL(statusChanged(QCamera::Status)),
            this, SLOT(statusChanged(QCamera::Status)));

    connect(mCapture, SIGNAL(captureDestinationChanged(QCameraImageCapture::CaptureDestinations)),
            this, SLOT(captureDestinationChanged(QCameraImageCapture::CaptureDestinations)));
    connect(mCapture, SIGNAL(bufferFormatChanged(QVideoFrame::PixelFormat)),
            this, SLOT(bufferFormatChanged(QVideoFrame::PixelFormat)));
    connect(mCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)),
            this, SLOT(error(int,QCameraImageCapture::Error,QString)));
    connect(mCapture, SIGNAL(imageAvailable(int,QVideoFrame)),
            this, SLOT(imageAvailable(int,QVideoFrame)));
    connect(mCapture, SIGNAL(imageCaptured(int,QImage)),
            this, SLOT(imageCaptured(int,QImage)));
    connect(mCapture, SIGNAL(imageExposed(int)),
            this, SLOT(imageExposed(int)));
    connect(mCapture, SIGNAL(imageMetadataAvailable(int,QString,QVariant)),
            this, SLOT(imageMetadataAvailable(int,QString,QVariant)));
    connect(mCapture, SIGNAL(imageSaved(int,QString)),
            this, SLOT(imageSaved(int,QString)));
    connect(mCapture, SIGNAL(readyForCaptureChanged(bool)),
            this, SLOT(readyForCaptureChanged(bool)));

    mCamera->setCaptureMode(QCamera::CaptureStillImage);
    mCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
}

/*Slots*/
void CameraSampler::captureModeChanged(QCamera::CaptureModes mode){
    qDebug() << "Capture mode set to " << mode;
}
void CameraSampler::error(QCamera::Error value){
    qDebug() << "error " << value;
}
void CameraSampler::lockFailed(){
    qDebug() << "Lock Failed";
}
void CameraSampler::lockStatusChanged(QCamera::LockStatus status, QCamera::LockChangeReason reason){
    qDebug() << "Lock Status Changed to " << status <<" because " << reason;
}
void CameraSampler::locked(){
    qDebug() << "locked";
}
void CameraSampler::stateChanged(QCamera::State state){
    qDebug() << "State changes to " << state;
}
void CameraSampler::statusChanged(QCamera::Status status){
    qDebug() << "status changed to " << status;
}

void CameraSampler::bufferFormatChanged(QVideoFrame::PixelFormat format){
    qDebug() << "buffer format changed to " << format;
}
void CameraSampler::captureDestinationChanged(QCameraImageCapture::CaptureDestinations destination){
    qDebug() << "capture destination changed to " << destination;
}
void CameraSampler::error(int id, QCameraImageCapture::Error error, const QString & errorString){
    qDebug() << "error " << error << " : " << errorString;
}
void CameraSampler::imageAvailable(int id, const QVideoFrame & buffer){
    qDebug() << "image Available";
}
void CameraSampler::imageCaptured(int id, const QImage & preview){
    qDebug() << "image Captured";
}
void CameraSampler::imageExposed(int id){
    qDebug() << "image Exposed";
}
void CameraSampler::imageMetadataAvailable(int id, const QString & key, const QVariant & value){
    qDebug() << "image Meta available ... osef";
}
void CameraSampler::imageSaved(int id, const QString & fileName){
    qDebug() << "Image Saved " << fileName;
}
void CameraSampler::readyForCaptureChanged(bool ready){
    qDebug() << "Ready for capture: " << ready;
}

/*Methods*/
QByteArray CameraSampler::getSample(){
    mCamera->start();
    mCamera->searchAndLock(QCamera::LockFocus);
    mCapture->capture();
    mCamera->unlock();
    return QByteArray();
}

/*Destructor*/
CameraSampler::~CameraSampler(){
    delete mImage, mCamera, mCapture;
}
