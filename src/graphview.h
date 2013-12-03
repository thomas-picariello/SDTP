#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QList>
#include <QLabel>
#include <QPoint>
#include <QTime>
#include <QDebug>
#include "soundsampler.h"

class GraphView : public QWidget
{
    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = 0);
    ~GraphView();

signals:
    
public slots:
    void paintEvent(QPaintEvent *);
    void sampleHandler(QByteArray sample);

private:
    SoundSampler *mSoundSampler;
    QTimer *mTimer;
    QImage *mImg;
    QLabel *mImgLabel;
};

#endif //GRAPHVIEW_H
