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

class Handshaker : public QObject
{
    Q_OBJECT

public:
    static const byte SUPPORTED_PROTOCOL_VERSION = 0x01;

    enum Error: byte{
        UndefinedError = 0x00,
        BadPrivateKey = 0x01,
        BadContactKey = 0x02,
        BadSymmetricKey = 0x03,
        BadSecurityLevel = 0x04,
        IncompatibleProtocolVersions = 0x05,
        IdentityCheckFailed = 0x06,
        IntegrityCheckFailed = 0x07,
        DataCorrupted = 0x08
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

    explicit Handshaker(TcpLink *link, QPair<QByteArray, QByteArray> *fileKey, QObject *parent = 0);

    void beginStarterHandshake(Contact *contact);
    void beginResponderHandshake(ContactDB *contactDB);
    CryptoPP::GCM<CryptoPP::AES>::Encryption* getGcmEncryptor() const;
    CryptoPP::GCM<CryptoPP::AES>::Decryption* getGcmDecryptor() const;
    Contact* getContact() const;
    QString getErrorString(Error err) const;

signals:
    void error(Handshaker::Error);
    void success();
    void handshakeFinished(bool success);
    void newContactId(int id);

private slots:
    void responderParseStarterHello();
    void starterParseResponderHello();
    void responderParseHalfKeyAndResponderIntegrity();
    void starterParseStarterIntegrity();
    void responderParseHandshakeFinished();

private:
    QTimer m_Timeout;
    TcpLink *m_Link;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    CryptoPP::AutoSeededRandomPool m_RandomGenerator;
    CryptoPP::GCM<CryptoPP::AES>::Encryption *m_GcmEncryptor;
    CryptoPP::GCM<CryptoPP::AES>::Decryption *m_GcmDecryptor;
    CryptoPP::RSAES_OAEP_SHA_Encryptor m_RsaEncryptor;
    CryptoPP::RSAES_OAEP_SHA_Decryptor m_RsaDecryptor;
    RsaKeyring m_RsaKeyring;
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
    QByteArray rsaDecrypt(QByteArray& cipherText);
    QByteArray rsaEncrypt(QByteArray& clearText);
    QList<QByteArray*> splitData(QByteArray &data, uint chunkSize);
    void updateIntegrityHash(QByteArray *currentHash, const QByteArray &data);


    Q_DISABLE_COPY(Handshaker)
};

#endif // HANDSHAKER_H
