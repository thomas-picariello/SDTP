#ifndef VOIP_H
#define VOIP_H

#include <QDebug>
#include <QIODevice>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <opus/opus.h>
#include "qpcmbuffer.h"

class VoIP : public QIODevice
{
    Q_OBJECT

public:
    explicit VoIP(QIODevice *parent = 0);

    void start();
    void stop();

    QAudioFormat getAudioFormat() const;
    void setAudioFormat(QAudioFormat format);
    float getOpusFrameSize() const;
    void setOpusFrameSize(float frameSizeInMs);
    int getEncoderApplication() const;
    void setEncoderApplication(int application);
    quint64 getBitrate() const;
    void setBitrate(quint64 bitrate);

    ~VoIP();

signals:
    
public slots:
    void opusEncode();
    void opusDecode();

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 size);

private:
    QAudioInput *mAudioInput;
    QAudioOutput *mAudioOutput;
    QAudioFormat mAudioFormat;
    OpusEncoder *mEncoder;
    OpusDecoder *mDecoder;
    QPcmBuffer mInputPcmBuffer;
    QPcmBuffer mOutputPcmBuffer;
    QByteArray mInputEncodedBuffer;
    QByteArray mOutputEncodedBuffer;
    int mApplication;
    float mOpusFrameLength;

    //Debug
    void displayOpusErr(int err);

    Q_DISABLE_COPY(VoIP)
};

#endif // VOIP_H
