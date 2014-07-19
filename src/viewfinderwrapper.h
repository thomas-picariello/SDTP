#ifndef VIEWFINDERWRAPPER_H
#define VIEWFINDERWRAPPER_H

#include <QDeclarativeItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QCamera>
#include <QPixmap>
#include <QCameraViewfinder>
#include "videosurface.h"
#include "videoencoder.h"

class ViewFinderWrapper : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(int cameraStatus READ cameraStatus NOTIFY cameraStatusChanged)
    Q_PROPERTY(int cameraError READ cameraError NOTIFY cameraErrorChanged)
    Q_PROPERTY(long frameCount READ frameCount NOTIFY frameCountChanged)
    Q_PROPERTY(long processedCount READ processedCount NOTIFY processedCountChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
public:
    explicit ViewFinderWrapper(QDeclarativeItem *parent = 0);
    virtual ~ViewFinderWrapper();
signals:
    void cameraStatusChanged(int status);
    void cameraErrorChanged(int error);
    void frameCountChanged(long count);
    void processedCountChanged(long count);
    void runningChanged();
    void newFrameAvaillable(QImage*);
    void newFrameToSend(QByteArray);

public slots:
    int cameraStatus() const;
    int cameraError() const;
    long frameCount() const;
    long processedCount() const;

    bool running() const;
    void startCamera();
    void stopCamera();

private slots:
    // Camera
    void onStateChanged(QCamera::State state);
    void onStatusChanged(QCamera::Status status);
    void onCameraError(QCamera::Error error);

    // Surface
    void onNewFrame(QImage *frame);

    // ProcessingThread
    void onThreadCongested();

private:
    void reset();
    void startViewFinder();
    void stopViewFinder();



private:
    QCamera* m_camera;
    QCamera::Status m_cameraStatus;
    QCamera::Error m_cameraError;
    bool m_cameraActive;

    VideoSurface m_surface;
    bool m_viewFinderActive;

    long m_receivedFrameCounter;
    long m_processedFrameCounter;

    VideoEncoder* m_processor;
};

#endif // VIEWFINDERWRAPPER_H
