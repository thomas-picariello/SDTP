#ifndef QOPUSENCODER_H
#define QOPUSENCODER_H

#include <QObject>
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QByteArray>
#include "opus/include/opus.h"

class QOpusDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QOpusDevice(QIODevice* deviceToUse, QObject* parent = 0);
    bool open(OpenMode mode);
    void close();
    bool isSequential() const;
    ~QOpusDevice();

signals:
    void error(int);

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * input, qint64 maxSize);

private:
    OpusEncoder *mEncoder;
    OpusDecoder *mDecoder;
    QIODevice *mUnderlyingDevice;
    QByteArray mInputBuffer, mOutputBuffer;
    int mFrameSizePerChannel;
    int mFrameSizeInByte;
    int mChannelsNumber;
    int mError;

    Q_DISABLE_COPY(QOpusDevice)
};

#endif // QOPUSENCODER_H
