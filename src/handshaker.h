#ifndef HANDSHAKER_H
#define HANDSHAKER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/rsa.h>

#include "tcplink.h"
#include "contact.h"
#include "contactdb.h"
#include "rsakeyring.h"

class Handshaker : public QObject
{
    Q_OBJECT
public:
    enum Error: byte{
        UndefinedError = 0x00,
        NoSupportedVersionAvailable = 0x01,
        IdentityCheckFailed = 0x02,
        IntegrityCheckFailed = 0x03
    };

    enum SecurityLevel: byte{
        UnverifiedIdentity = 0x00,
        ThirdPartyVerifiedIdentity = 0x01,
        PreSharedIdentity = 0x02
    };

    explicit Handshaker(TcpLink *link, QPair<QByteArray, QByteArray> *fileKey, QObject *parent = 0);

    void beginStarterHandshake(Contact *contact);
    void beginResponderHandshake(ContactDB *contactDB);
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption* getAesEncryptor() const;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption* getAesDecryptor() const;
    uint getSecurityLevel() const;
    void setSecurityLevel(uint level);

signals:
    void error(Handshaker::Error);
    void success();
    void handshakeFinished(bool success);

public slots:
    void sayHello();
    void recieveStarterHello();
    void respondHello();
    void recieveResponderHello();
    void sendHalfKeyAndPartnerIntegrity();
    void recieveHalfKeyAndPartnerIntegrity();
    void sendPartnerIntegrity();
    void recievePartnerIntegrity();
    void sendHandshakeFinished();
    void recieveHandshakeFinished();
    void sendError();

private:
    QTimer m_Timeout;
    TcpLink *m_Link;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    CryptoPP::AutoSeededRandomPool m_RandomGenerator;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption *m_AesEncryptor;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption *m_AesDecryptor;
    CryptoPP::RSAES_OAEP_SHA_Encryptor m_RsaEncryptor;
    CryptoPP::RSAES_OAEP_SHA_Decryptor m_RsaDecryptor;
    RsaKeyring m_RsaKeyring;
    QByteArray m_MyIntegrityHash;
    QByteArray m_PartnerIntegrityHash;
    QByteArray m_SupportedVersions;
    bool m_HandshakeFinished;
    uint m_SecurityLevel;

    QByteArray genHalfSymKey();
    void updateIntegrityHash(QByteArray *currentHash, const QByteArray &data);
    bool isError() const;
    QList<QByteArray*> splitData(QByteArray &data, uint chunkSize);

    Q_DISABLE_COPY(Handshaker)
};

#endif // HANDSHAKER_H
