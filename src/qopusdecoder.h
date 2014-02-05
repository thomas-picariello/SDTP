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
    explicit QOpusDecoder(QIODevice* parent = 0);

    bool isSequential() const;
    quint64 getBufferMaxSize() const;
    void setBufferMaxSize(quint64 bytesCount);

    static QString getOpusErrorDesc(int errorCode);

    ~QOpusDecoder();

signals:
    void error(const int err);

public slots:
    void decode();


protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private:
    quint64 mBufferMaxSize;
    OpusDecoder *mDecoder;
    QByteArray mRawBuffer;
    QByteArray mEncodedBuffer;
    QAudioFormat mAudioFormat;
    int mError;

    Q_DISABLE_COPY(QOpusDecoder)
};

#endif // QOPUSDECODER_H
