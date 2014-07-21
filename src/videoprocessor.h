#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QQueue>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QVideoFrame>

class VideoProcessor : public QObject {
    Q_OBJECT

public:
    VideoProcessor();
    ~VideoProcessor();


public slots:
    void process(QVideoFrame frame);
    void decode(QByteArray array);
    void stop();

signals:
    void finished(QByteArray);
    void imgForDisplay(QImage);
    void decoded(QImage);
    void error(QString err);

private:
    int i;


    // add your variables here
};
#endif // VIDEOPROCESSOR_H
