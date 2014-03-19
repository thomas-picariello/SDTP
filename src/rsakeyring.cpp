#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QByteArray *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    connect(&mWatcher, SIGNAL(finished()),
            this, SLOT(onKeyGenJobFinished()));
}

void RsaKeyring::exportKeys(QString filePath){

}

void RsaKeyring::exportPublicKey(QString filePath){

}

void RsaKeyring::generateKeypair(){
    QFuture<QPair<QByteArray,QByteArray>> generateFuture = QtConcurrent::run(this, &RsaKeyring::generate);
    mWatcher.setFuture(generateFuture);
}

QByteArray* RsaKeyring::getPrivateKey(){
    return &mPrivateKey;
}

QByteArray *RsaKeyring::getPublicKey(){
    return &mPublicKey;
}

void RsaKeyring::importKeys(QString filePath){

}

void RsaKeyring::importPublicKey(QString filePath){

}

void RsaKeyring::onKeyGenJobFinished(){
    mPrivateKey = mWatcher.result().first;
    mPublicKey = mWatcher.result().second;
    emit keyGenerationFinished();
}

void RsaKeyring::setPrivateKey(QByteArray privateKey){

}

void RsaKeyring::setPublicKey(QByteArray publicKey){

}

QPair<QByteArray, QByteArray> RsaKeyring::generate(){
    std::string privateKey;
    std::string publicKey;
    CryptoPP::StringSink privateSink(privateKey);
    CryptoPP::StringSink publicSink(publicKey);
    CryptoPP::ByteQueue queue;

    try{
        CryptoPP::RSAES_OAEP_SHA_Decryptor rsaPrivate;
        rsaPrivate.AccessKey().GenerateRandomWithKeySize(CryptoPP::AutoSeededRandomPool(), 2048);
        CryptoPP::RSAES_OAEP_SHA_Encryptor rsaPublic(rsaPrivate);
        rsaPrivate.AccessKey().Save(queue);
        queue.CopyTo(privateSink);
        rsaPublic.AccessKey().Save(queue);
        queue.CopyTo(publicSink);
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
    }
    return QPair<QByteArray, QByteArray>(QString::fromStdString(privateKey).toUtf8(),
                                         QString::fromStdString(publicKey).toUtf8());
}

RsaKeyring::~RsaKeyring(){

}
