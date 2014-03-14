#ifndef RSAKEYRING_H
#define RSAKEYRING_H

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>

class RsaKeyring : public QObject
{
    Q_OBJECT
public:
    explicit RsaKeyring(QByteArray *fileKey,
                        QObject *parent=0);
    ~RsaKeyring();

    void exportKeys(QString filePath);
    void exportPublicKey(QString filePath);
    void generate();
    QByteArray getPrivateKey();
    QByteArray getPublicKey();
    void importKeys(QString filePath);
    void importPublicKey(QString filePath);
    void setPrivateKey(QByteArray privateKey);
    void setPublicKey(QByteArray publicKey);

private:
    QByteArray *mFileKey;
    QByteArray mPrivateKey;
    QByteArray mPublicKey;

    void encodeBase64(QByteArray key);

    Q_DISABLE_COPY(RsaKeyring)
};

#endif //RSAKEYRING_H
