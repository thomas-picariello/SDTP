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
    explicit QOpusEncoder(int frameSizeInMicrosecs = 200,
                         QIODevice* parent = 0);

    bool isSequential() const;

    int getFrameSize() const;
    void setFrameSize(int frameSizeInMicrosecs);
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
    QByteArray mBuffer;
    QAudioFormat mAudioFormat;
    int mError;
    int mOpusFrameSize;
    int mApplication;

    Q_DISABLE_COPY(QOpusEncoder)
};

#endif // QOPUSENCODER_H
