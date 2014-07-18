#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <QThread>
#include <QImage>
#include <QQueue>
#include <QVideoFrame>

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
    void addFrameToProcessingQueue(QVideoFrame frame);
private:
    virtual void run();
private:
    QQueue<QVideoFrame> m_queue;
    int m_queueMaxLength;
    bool m_stopped;
};

#endif // VIDEOENCODER_H
