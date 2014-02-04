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
    bool open();
    void close();
    bool isSequential() const;

    static QString getOpusErrorDesc(int errorCode);

    ~QOpusDecoder();

signals:
    void error(const int err);

public slots:


protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private:
    OpusDecoder *mDecoder;
    QByteArray mBuffer;
    QAudioFormat mAudioFormat;
    int mError;

    Q_DISABLE_COPY(QOpusDecoder)
};

#endif // QOPUSDECODER_H
