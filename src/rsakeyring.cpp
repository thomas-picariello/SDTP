#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    readKeystore();
    connect(&mWatcher, SIGNAL(finished()),
            this, SLOT(onKeyGenJobFinished()));
}

void RsaKeyring::commitToKeystore(){
    if(validateKeypair()){
        QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
        QFile keystoreFile("keystore.xml");
        keystoreFile.open(QFile::WriteOnly);
        keystoreFile.flush();
        QXmlStreamWriter xml(&keystoreFile);
        xml.setAutoFormatting(true);

        xml.writeStartDocument();
        xml.writeStartElement("keyring");
        xml.writeAttribute("created", timestamp);
        xml.writeTextElement("public_key", mPublicKey.toBase64());
        xml.writeTextElement("private_key", mPrivateKey.toBase64());
        xml.writeEndDocument();

        keystoreFile.close();
    }
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
    mPrivateKey = privateKey;
}

void RsaKeyring::setPublicKey(QByteArray publicKey){
    mPublicKey = publicKey;
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

bool RsaKeyring::validateKeypair(){
    if(!mPublicKey.isEmpty() && !mPrivateKey.isEmpty())
        return true;
    else
        return false;
}

void RsaKeyring::readKeystore(){
    QFile keystoreFile("keystore.xml");
    if(keystoreFile.exists()){
        keystoreFile.open(QFile::ReadOnly);
        QXmlStreamReader reader(&keystoreFile);
        while(!reader.atEnd()){
            if (reader.isStartElement()){
                if (reader.name() == "keyring"){
                    reader.readNext();
                    while(!reader.isStartElement() && !reader.atEnd())
                        reader.readNext();
                    if(reader.name() == "public_key")
                        mPublicKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                    while(!reader.isStartElement() && !reader.atEnd())
                        reader.readNext();
                    if(reader.name() == "private_key")
                        mPrivateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                }
            }
            reader.readNext();
        }
    }
    keystoreFile.close();
    //TODO: Verify key consistency
}

RsaKeyring::~RsaKeyring(){

}
