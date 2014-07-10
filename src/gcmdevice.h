#ifndef GCMDEVICE_H
#define GCMDEVICE_H

#include <QDebug>
#include <QIODevice>
#include <QPair>
#include <QtEndian>

#include <cryptopp/cryptlib.h>

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/filters.h>
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::AuthenticatedDecryptionFilter;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AAD_CHANNEL;
using CryptoPP::DEFAULT_CHANNEL;
using CryptoPP::HashFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include <cryptopp/sha.h>
using CryptoPP::SHA256;

#include <cryptopp/gcm.h>
using CryptoPP::GCM;

#include "abstractlink.h"

class GcmDevice : public QIODevice
{
    Q_OBJECT
public:
    enum Error{

    };

    explicit GcmDevice(AbstractLink* link, QObject *parent = 0);

    AbstractLink* getLink();
    void setLink(AbstractLink* link, bool destroyOld = true);
    QByteArray getKey() const;
    QByteArray getBaseIV() const;
    void setKeyAndBaseIV(QByteArray key, QByteArray baseIV);

    virtual bool isSequential(){ return true; }
    virtual bool open(OpenMode mode);

private slots:
    void onLinkOpenModeChanged(OpenMode openMode);
    void readFromLink();

private:
    static const int GCM_TAG_SIZE = 16;
    QByteArray m_BaseIV;
    QByteArray m_DataBuffer;
    QByteArray m_Key;
    quint64 m_LastSequenceNumber;
    AbstractLink* m_Link;
    QDataStream m_LinkStream;
    QList<QPair<quint64,QByteArray>> m_PacketList;

    QByteArray generateIV(quint64 sequenceNumber);
    QByteArray decrypt(quint64 seqNum, QByteArray& gcmPacket);
    QByteArray encrypt(quint64 seqNum, QByteArray& clearText);
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    Q_DISABLE_COPY(GcmDevice)
};

#endif // GCMDEVICE_H
