#ifndef GCMDEVICE_H
#define GCMDEVICE_H

#include <QDebug>
#include <QIODevice>
#include <QPair>
#include <QtEndian>

#include <cryptopp/cryptlib.h>
using CryptoPP::g_nullNameValuePairs;

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

private:
    quint64 m_LastSequenceNumber;
    AbstractLink* m_Link;
    QByteArray m_Key;
    QByteArray m_BaseIV;

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
    QByteArray generateIV(quint64 sequenceNumber);

    Q_DISABLE_COPY(GcmDevice)
};

#endif // GCMDEVICE_H
