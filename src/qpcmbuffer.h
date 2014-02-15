#ifndef QPCMBUFFER_H
#define QPCMBUFFER_H

#include <QDebug>
#include <QIODevice>
#include <QVector>
#include <QtEndian>

class QPcmBuffer : public QIODevice
{
    Q_OBJECT
public:
    explicit QPcmBuffer(QIODevice* parent = 0);
    explicit QPcmBuffer(qint64 bufferMaxSize, QIODevice* parent = 0);
    explicit QPcmBuffer(QVector<qint16> *buffer, QIODevice* parent = 0);

    QVector<qint16>& buffer();
    void setBuffer(QVector<qint16> *buffer);
    qint16* preAllocate(qint64 size);
    qint16* data();
    const qint16* data() const;
    void setData(const QVector<qint16> &data);
    qint64 size() const;
    qint64 getMaxSize() const;
    void setMaxSize(qint64 maxSize);

    ~QPcmBuffer();

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 size);

private:
    QVector<qint16> *mBuffer;
    qint64 mBufferMaxSize;
    bool hasExternalBuffer;

    void free(qint64 size);

    Q_DISABLE_COPY(QPcmBuffer)
};

#endif // QPCMBUFFER_H
