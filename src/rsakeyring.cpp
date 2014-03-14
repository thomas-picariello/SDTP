#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QByteArray *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{

}

void RsaKeyring::exportKeys(QString filePath){

}

void RsaKeyring::exportPublicKey(QString filePath){

}

void RsaKeyring::generate(){
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSAES_OAEP_SHA_Decryptor rsaPrivate;
    rsaPrivate.AccessKey().GenerateRandomWithKeySize(rng, 4096);
    CryptoPP::RSAES_OAEP_SHA_Encryptor rsaPublic(rsaPrivate);
}

QByteArray RsaKeyring::getPrivateKey(){
    return QByteArray();
}

QByteArray RsaKeyring::getPublicKey(){
    return QByteArray();
}

void RsaKeyring::importKeys(QString filePath){

}

void RsaKeyring::importPublicKey(QString filePath){

}

void RsaKeyring::setPrivateKey(QByteArray privateKey){

}

void RsaKeyring::setPublicKey(QByteArray publicKey){

}

RsaKeyring::~RsaKeyring(){

}
