#ifndef OPUSVOICECODEC_H
#define OPUSVOICECODEC_H

#include <QDebug>
#include <QIODevice>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <opus/opus.h>
#include "qpcmbuffer.h"

class OpusVoiceCodec : public QIODevice
{
    Q_OBJECT

public:
    explicit OpusVoiceCodec(QIODevice *parent = 0);
    ~OpusVoiceCodec();

    QAudioFormat getAudioFormat() const;
    void setAudioFormat(QAudioFormat format);
    float getOpusFrameSize() const;
    void setOpusFrameSize(float frameSizeInMs);
    int getEncoderApplication() const;
    void setEncoderApplication(int application);
    quint64 getBitrate() const;
    void setBitrate(quint64 bitrate);
    void getProbes() const;    

signals:

public slots:
    void start();
    void stop();

private slots:
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

    Q_DISABLE_COPY(OpusVoiceCodec)
};

#endif // OPUSVOICECODEC_H
