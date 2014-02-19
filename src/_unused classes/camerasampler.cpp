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

    connect(mCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)),
            this, SLOT(error(int,QCameraImageCapture::Error,QString)));
    connect(mCapture, SIGNAL(imageAvailable(int,QVideoFrame)),
            this, SLOT(imageAvailable(int,QVideoFrame)));
    connect(mCapture, SIGNAL(imageCaptured(int,QImage)),
            this, SLOT(imageCaptured(int,QImage)));
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
void CameraSampler::error(int id, QCameraImageCapture::Error error, const QString & errorString){

}

void CameraSampler::imageAvailable(int id, const QVideoFrame & buffer){
    qDebug() << "image Available";
}
void CameraSampler::imageCaptured(int id, const QImage & preview){
    qDebug() << "image Captured";
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
