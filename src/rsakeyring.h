#ifndef RSAKEYRING_H
#define RSAKEYRING_H

#include <QDebug>
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
    explicit RsaKeyring(QPair<QByteArray, QByteArray> *fileKey,
                        QObject *parent=0);

    void updateFileKey(QByteArray *oldKey = NULL);
    void commitToKeystore();
    void exportPrivateKey(QString filename);
    void exportPublicKey(QString filename);
    void generateKeypair();
    void generatePublicKey();
    QByteArray getPrivateKey() const;
    QByteArray getPublicKey() const;
    void importPrivateKey(QString filename);
    bool isGenerating() const;
    void setPrivateKey(QByteArray privateKey);
    bool validatePrivateKey(QByteArray privateKey, int level = 2);

signals:
    void keyGenerationFinished();
    void error(QString err);

public slots:
    void onPrivateKeyGenJobFinished();

private:
    QPair<QByteArray,QByteArray> *mFileKey;
    QByteArray mPrivateKey;
    QByteArray mPublicKey;
    QFutureWatcher<QByteArray> mGenerateWatcher;

    QByteArray generatePrivateKeyRunnable();
    void readKeystore();

    Q_DISABLE_COPY(RsaKeyring)
};

#endif //RSAKEYRING_H
