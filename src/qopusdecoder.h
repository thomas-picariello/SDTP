#ifndef QOPUSDECODER_H
#define QOPUSDECODER_H

#include <QDebug>
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QByteArray>
#include <opus/opus.h>

class QOpusDecoder : public QIODevice
{
    Q_OBJECT
public:
    explicit QOpusDecoder(QAudioFormat outputFormat,
                          QIODevice* parent = 0);

    bool isSequential() const;

    QAudioFormat getOutputAudioFormat() const;
    void setOutputAudioFormat(QAudioFormat outputFormat);
    quint64 getBufferMaxSize() const;
    void setBufferMaxSize(quint64 bytesCount);
    float getOpusFrameSize() const;
    void setOpusFrameSize(float frameSizeInMs);

    static QString getOpusErrorDesc(int errorCode);

    ~QOpusDecoder();

signals:
    void error(int err);

public slots:
    void decode();


protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 size);

private:
    quint64 mBufferMaxSize;
    OpusDecoder *mDecoder;
    QVector<qint16> mPcmBuffer;
    QVector<uchar> mEncodedBuffer;
    QAudioFormat mOpusAudioFormat;
    QAudioFormat mOutputAudioFormat;
    float mOpusFrameLength;

    Q_DISABLE_COPY(QOpusDecoder)
};

#endif // QOPUSDECODER_H
