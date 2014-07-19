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


public:
    explicit ViewFinderWrapper(QDeclarativeItem *parent = 0);
    virtual ~ViewFinderWrapper();
signals:
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
    // Surface
    void onNewFrame(QImage *frame);


private:
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
