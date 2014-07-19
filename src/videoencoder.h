#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <QThread>
#include <QImage>
#include <QQueue>
#include <QVideoFrame>
#include <QDataStream>
#include <QCameraViewfinder>

#include "videosurface.h"

class VideoEncoder : public QThread
{
    Q_OBJECT
public:
    explicit VideoEncoder(QObject *parent = 0);
    virtual ~VideoEncoder();
    QImage *img;
signals:
    void frameProcessed(QByteArray);
    void imgForDisplay(QImage*);
public:
    void stop();
public :
    void addFrameToProcessingQueue(QImage *frame);
private:
    virtual void run();
private:
    QQueue<QImage> m_queue;
    int m_queueMaxLength;
    bool m_stopped;


};

#endif // VIDEOENCODER_H
