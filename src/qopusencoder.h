#ifndef QOPUSENCODER_H
#define QOPUSENCODER_H

#include <QDebug>
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QByteArray>
#include <opus/opus.h>

class QOpusEncoder : public QIODevice
{
    Q_OBJECT
public:
    explicit QOpusEncoder(float frameSizeInMs = 20.0,
                         QIODevice* parent = 0);

    bool isSequential() const;

    float getBufferLength() const;
    void setBufferLength(float lengthInMs);
    float getOpusFrameSize() const;
    void setOpusFrameSize(float frameSizeInMs);
    int getEncoderApplication() const;
    void setEncoderApplication(int application);
    quint64 getBitrate() const;
    void setBitrate(quint64 bitrate);

    static QString getOpusErrorDesc(int errorCode);

    ~QOpusEncoder();

signals:
    void error(const int err);

public slots:
    void encode();

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private:
    OpusEncoder *mEncoder;
    QByteArray mRawBuffer;
    QAudioFormat mAudioFormat;
    int mError;
    float mOpusFrameSize;
    float mBufferLength;
    int mApplication;

    Q_DISABLE_COPY(QOpusEncoder)
};

#endif // QOPUSENCODER_H
