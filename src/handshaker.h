#ifndef HANDSHAKER_H
#define HANDSHAKER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/authenc.h>
#include <cryptopp/gcm.h>
#include <cryptopp/rsa.h>

#include "tcplink.h"
#include "contact.h"
#include "contactdb.h"
#include "rsakeyring.h"
#include "ipfilter.h"

class Handshaker : public QObject
{
    Q_OBJECT

public:
    static const byte SUPPORTED_PROTOCOL_VERSION = 0x01;

    enum Mode{
        UndefinedMode,
        StarterMode,
        ResponderMode
    };

    enum Error: byte{
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

    enum Success: byte{
        HandshakeFinished = 0x10
    };
    Q_ENUMS(Success)

    enum SecurityLevel: byte{
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

    quint16 getBanTime() const;
    Contact* getContact() const;
    QString getErrorString(Error err) const;
    CryptoPP::GCM<CryptoPP::AES>::Encryption* getGcmEncryptor() const;
    CryptoPP::GCM<CryptoPP::AES>::Decryption* getGcmDecryptor() const;
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
    quint16 m_BanTime;
    Mode m_Mode;
    QTimer m_Timeout;
    TcpLink *m_Link;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    IpFilter *m_IpFilter;
    RsaKeyring *m_RsaKeyring;
    CryptoPP::AutoSeededRandomPool m_RandomGenerator;
    CryptoPP::GCM<CryptoPP::AES>::Encryption *m_GcmEncryptor;
    CryptoPP::GCM<CryptoPP::AES>::Decryption *m_GcmDecryptor;
    CryptoPP::RSAES_OAEP_SHA_Encryptor m_RsaEncryptor;
    CryptoPP::RSAES_OAEP_SHA_Decryptor m_RsaDecryptor;
    QPair<QByteArray,QByteArray> m_GcmKey;
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
    bool isError(const QByteArray &data);
    QByteArray rsaDecrypt(QByteArray& cipherText);
    QByteArray rsaEncrypt(QByteArray& clearText);
    QList<QByteArray*> splitData(const QByteArray &data, const uint chunkSize);
    void updateIntegrityHash(QByteArray *currentHash, const QByteArray &data);

    Q_DISABLE_COPY(Handshaker)
};

#endif // HANDSHAKER_H
