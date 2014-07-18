#ifndef OPUSVOICECODEC_H
#define OPUSVOICECODEC_H

#include <QDebug>
#include <QIODevice>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QtMath>

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
    void setOutputVolume(qreal volume);
    void setInputMute(bool mute);
    void setOutputMute(bool mute);

signals:
    void newInputProbe(qint16 inProbe);
    void newOutputProbe(qint16 outProbe);

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
    QAudioInput *m_audioInput;
    QAudioOutput *m_audioOutput;
    QAudioFormat m_audioFormat;
    OpusEncoder *m_encoder;
    OpusDecoder *m_decoder;
    QPcmBuffer m_inputPcmBuffer;
    QPcmBuffer m_outputPcmBuffer;
    QByteArray m_inputEncodedBuffer;
    QByteArray m_outputEncodedBuffer;
    int m_application;
    float m_opusFrameLength;

    qint16 computeRMS(const QPcmBuffer& buffer);
    //Debug
    void displayOpusErr(int err);

    Q_DISABLE_COPY(OpusVoiceCodec)
};

#endif // OPUSVOICECODEC_H
