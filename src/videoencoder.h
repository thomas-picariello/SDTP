#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <QThread>
#include <QImage>
#include <QQueue>
#include <QVideoFrame>
#include <QBuffer>
#include <QDataStream>

class VideoEncoder : public QThread
{
    Q_OBJECT
public:
    explicit VideoEncoder(QObject *parent = 0);
    virtual ~VideoEncoder();
signals:
    void frameProcessed(QByteArray);
    void queueFull();
public:
    void stop();
    void addFrameToProcessingQueue(QImage *frame);
private:
    virtual void run();
private:
    QQueue<QImage> m_queue;
    QImage *m_img;
    int m_queueMaxLength;
    bool m_stopped;
};

#endif // VIDEOENCODER_H
