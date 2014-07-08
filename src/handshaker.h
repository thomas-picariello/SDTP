#ifndef HANDSHAKER_H
#define HANDSHAKER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QDataStream>

#include <cryptopp/cryptlib.h>

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/filters.h>
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;
using CryptoPP::PK_DecryptorFilter;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::HashFilter;

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

#include <cryptopp/gcm.h>
using CryptoPP::GCM;

#include <cryptopp/rsa.h>
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;

#include <cryptopp/sha.h>
using CryptoPP::SHA256;

#include "tcplink.h"
#include "contact.h"
#include "contactdb.h"
#include "rsakeyring.h"
#include "ipfilter.h"

class Handshaker : public QObject
{
    Q_OBJECT

public:
    static const quint8 SUPPORTED_PROTOCOL_VERSION = 0x01;

    enum Mode{
        UndefinedMode,
        StarterMode,
        ResponderMode
    };

    enum Error: quint8{
        UndefinedError = 0x00,
        BadPrivateKey = 0x01,
        BadContactKey = 0x02,
        BadSymmetricKey = 0x03,
        BadSecurityLevel = 0x04,
        IncompatibleProtocolVersions = 0x05,
        IdentityCheckFailed = 0x06,
        IntegrityCheckFailed = 0x07,
        DataCorrupted = 0x08,
        Timeout = 0x09
    };
    Q_ENUMS(Error)

    enum Success: quint8{
        HandshakeFinished = 0x10
    };
    Q_ENUMS(Success)

    enum SecurityLevel: quint8{
        UnverifiedIdentity = 0x00,
        ThirdPartyVerifiedIdentity = 0x01,
        PreSharedIdentity = 0x02
    };
    Q_ENUMS(SecurityLevel)

    explicit Handshaker(TcpLink *link,
                        RsaKeyring *keyring,
                        QObject *parent = 0);

    void beginStarterHandshake(Contact *contact);
    void waitForHandshake(ContactDB *contactDB);

    quint16 getRecievedBanTime() const;
    QByteArray getGcmBaseIV() const{ return m_GcmBaseIV; }
    Contact* getContact() const;
    QString getErrorString(Error err) const;
    QByteArray getGcmKey() const{ return m_GcmKey; }
    Mode getMode() const;
    void setIpFilter(IpFilter *ipFilter);
    void setTimeout(int timeout);

signals:
    void error(Handshaker::Error);
    void handshakeFinished(bool success);
    void newContactId(int id);

public slots:
    void resetHandshake();

private slots:
    void responderParseStarterHello();
    void starterParseResponderHello();
    void responderParseHalfKeyAndResponderIntegrity();
    void starterParseStarterIntegrity();
    void responderParseHandshakeFinished();
    void onTimeout();

private:
    byte m_IvOffset;
    quint16 m_RecievedBanTime;
    Mode m_Mode;
    QTimer m_Timeout;
    TcpLink *m_Link;
    QDataStream m_LinkStream;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    IpFilter *m_IpFilter;
    RsaKeyring *m_RsaKeyring;
    AutoSeededRandomPool m_RandomGenerator;
    RSAES_OAEP_SHA_Encryptor m_RsaEncryptor;
    RSAES_OAEP_SHA_Decryptor m_RsaDecryptor;
    QByteArray m_GcmKey;
    QByteArray m_GcmBaseIV;
    QByteArray m_StarterIntegrityHash;
    QByteArray m_ResponderIntegrityHash;


    void starterSayHello();
    void responderRespondHello();
    void starterSendHalfKeyAndResponderIntegrity();
    void responderSendStarterIntegrity();
    void starterSendHandshakeFinished();
    void processError(Error err);

    QByteArray generateRandomBlock(uint size);
    QByteArray gcmDecrypt(QByteArray& cipherText);
    QByteArray gcmEncrypt(QByteArray& clearText);
    bool isError(const QByteArray &packet);
    QByteArray rsaDecrypt(QByteArray& cipherText);
    QByteArray rsaEncrypt(QByteArray& clearText);
    QList<QByteArray*> splitData(const QByteArray &data, const uint chunkSize);
    void updateIntegrityHash(QByteArray *currentHash, const QByteArray &data);

    Q_DISABLE_COPY(Handshaker)
};

#endif // HANDSHAKER_H
