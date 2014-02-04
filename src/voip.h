#ifndef VOIP_H
#define VOIP_H

#include <QDebug>
#include <QIODevice>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include "qopusencoder.h"
#include "qopusdecoder.h"

class VoIP : public QIODevice
{
    Q_OBJECT

public:
    explicit VoIP(QIODevice *parent = 0);

    void start();
    void stop();
    ~VoIP();

signals:
    
public slots:
    //void startAudioOutput();
    //Debug
    void inputStateChanged(QAudio::State state);
    void outputStateChanged(QAudio::State state);

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private:
    QAudioInput *mAudioInput;
    QAudioOutput *mAudioOutput;
    QOpusEncoder *mOpusEncoder;
    QOpusDecoder *mOpusDecoder;
};

#endif // VOIP_H
