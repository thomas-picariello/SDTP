#ifndef CAMERASAMPLER_H
#define CAMERASAMPLER_H

#include <QObject>
#include <QByteArray>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraImageCapture>
#include <QImage>
#include <QDebug>

class CameraSampler : public QObject
{
    Q_OBJECT
public:
    explicit CameraSampler(QObject *parent = 0);
    ~CameraSampler();
    QByteArray getSample();
    
signals:

public slots:
    void captureModeChanged(QCamera::CaptureModes mode);
    void error(QCamera::Error value);

    void error(int id, QCameraImageCapture::Error error, const QString & errorString);
    void imageAvailable(int id, const QVideoFrame & buffer);
    void imageCaptured(int id, const QImage & preview);
    void readyForCaptureChanged(bool ready);

private:
    QCamera *mCamera;
    QCameraImageCapture *mCapture;
    QImage *mImage;
    float mPixelOffset;
};

#endif // CAMERASAMPLER_H
