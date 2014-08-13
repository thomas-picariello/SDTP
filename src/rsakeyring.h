/**
 * @brief RsaKeyring is used to store the Private and public key and/or generate some.
 */


#ifndef RSAKEYRING_H
#define RSAKEYRING_H

//#include <QDebug>
#include <QObject>
#include <QByteArray>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDateTime>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <cryptopp/authenc.h>
#include <cryptopp/gcm.h>
#include <cryptopp/files.h>

class RsaKeyring : public QObject
{
    Q_OBJECT
public:
    enum Error{
        PrivatKeyGenerationFailed,
        PublicKeyGenerationFailed,
        PrivateKeyValidationFailed,
        KeystoreLoadingFailed
    };
    Q_ENUMS(Error)

    explicit RsaKeyring(QPair<QByteArray, QByteArray> *fileKey,
                        QObject *parent=0);

    void updateFileKey(QByteArray &oldKey);
    QByteArray generatePublicKey(QByteArray &privateKey = QByteArray());
    void generateKeypair();
    QByteArray getStoredPrivateKey() const;
    bool hasPrivateKey() const;
    bool isGenerating() const;
    bool validatePrivateKey(QByteArray privateKey, int level = 2);

signals:
    void publicKeyGenerationFinished(QByteArray publicKey);
    void privateKeyGenerationFinished(QByteArray privatekey);
    void privateKeyValidated();
    void error(RsaKeyring::Error);

public slots:
    void commitToKeystore(QByteArray privateKey);

private slots:
    void onPrivateKeyGenJobFinished();

private:
    QPair<QByteArray,QByteArray> *mFileKey;
    QByteArray mStoredPrivateKey;
    QFutureWatcher<QByteArray> mGenerateWatcher;

    QByteArray generatePrivateKeyRunnable();
    void readKeystore();

    Q_DISABLE_COPY(RsaKeyring)
};

#endif //RSAKEYRING_H
