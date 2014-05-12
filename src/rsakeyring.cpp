#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    readKeystore();
    connect(&mWatcher, SIGNAL(finished()),
            this, SLOT(onKeyGenJobFinished()));
}

void RsaKeyring::changeFileKey(QPair<QByteArray,QByteArray> newKey){
    //TODO: reencrypt keystore.
}

void RsaKeyring::commitToKeystore(){
    //if(validatePrivateKey(mPrivateKey) && validatePublicKey(mPublicKey)){
        QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
        QString xmlString;
        QXmlStreamWriter xml(&xmlString);

        xml.writeStartDocument();
        xml.writeStartElement("keyring");
        xml.writeAttribute("created", timestamp);
        xml.writeTextElement("public_key", mPublicKey.toBase64());
        xml.writeTextElement("private_key", mPrivateKey.toBase64());
        xml.writeEndDocument();

        if(mFileKey->first.isEmpty()){
            QFile keystoreFile("keystore.dat");
            keystoreFile.open(QFile::WriteOnly|QFile::Truncate);
            keystoreFile.write(xmlString.toUtf8());
            keystoreFile.close();
        }else{
            std::string clearXml = xmlString.toStdString();
            CryptoPP::GCM<CryptoPP::AES>::Encryption gcmEnc;
            gcmEnc.SetKeyWithIV((byte*)mFileKey->first.data(), mFileKey->first.length(),    //key
                                (byte*)mFileKey->second.data(), mFileKey->second.length()); //iv
            CryptoPP::StringSource(clearXml, true,
                                       new CryptoPP::AuthenticatedEncryptionFilter(gcmEnc,
                                             new CryptoPP::FileSink("keystore.dat")));
        }
    //}
}

void RsaKeyring::exportPrivateKey(QString filename){
    QFile file(filename);
    if(file.open(QFile::Truncate|QFile::WriteOnly)){
        file.write(mPrivateKey);
        file.close();
    }else
        emit error(QString(tr("Unable to access the file:"))+filename);
}

void RsaKeyring::exportPublicKey(QString filename){
    QFile file(filename);
    if(file.open(QFile::Truncate|QFile::WriteOnly)){
        file.write(mPublicKey);
        file.close();
    }else
        emit error(QString(tr("Unable to access the file:"))+filename);
}

void RsaKeyring::generateKeypair(){
    QFuture<QPair<QByteArray,QByteArray>> generateFuture = QtConcurrent::run(this, &RsaKeyring::generate);
    mWatcher.setFuture(generateFuture);
}

QByteArray RsaKeyring::getPrivateKey() const{
    return mPrivateKey;
}

QByteArray RsaKeyring::getPublicKey() const{
    return mPublicKey;
}

void RsaKeyring::importPrivateKey(QString filename){
    QFile file(filename);
    if(file.open(QFile::ReadOnly)){
        mPrivateKey = file.readAll();
        file.close();
    }else
        emit error(QString(tr("Unable to access the file:")+filename));
}

void RsaKeyring::importPublicKey(QString filename){
    QFile file(filename);
    if(file.open(QFile::ReadOnly)){
        mPublicKey = file.readAll();
        file.close();
    }else
        emit error(QString(tr("Unable to access the file:")+filename));
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
        emit error(QString(e.what()));
    }
    return QPair<QByteArray, QByteArray>(QString::fromStdString(privateKey).toUtf8(),
                                         QString::fromStdString(publicKey).toUtf8());
}

bool RsaKeyring::validatePrivateKey(QByteArray privateKey, uint level){
    bool result;
    CryptoPP::RSA::PrivateKey rsaPrivate;
    std::string privKey = QString(privateKey).toStdString();
    CryptoPP::StringSource source(privKey, true);
    CryptoPP::ByteQueue queue;
    source.CopyTo(queue);

    try{
        rsaPrivate.BERDecodePrivateKey(source, false, source.MaxRetrievable());
        result = rsaPrivate.Validate(CryptoPP::AutoSeededRandomPool(), level);
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
        //emit error(QString(e.what()));
        result = false;
    }
    return result;
}

bool RsaKeyring::validatePublicKey(QByteArray publicKey, uint level){
    return true;
}

void RsaKeyring::readKeystore(){
    QByteArray privateKey, publicKey;
    std::string xmlString;
    try{
        if(mFileKey->first.isEmpty()){
            QFile keystoreFile("keystore.dat");
            keystoreFile.open(QFile::ReadOnly);
            xmlString = QString(keystoreFile.readAll()).toStdString();
            keystoreFile.close();
        }else{
            CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
            dec.SetKeyWithIV((byte*)mFileKey->first.data(), mFileKey->first.length(),     //key
                             (byte*)mFileKey->second.data(), mFileKey->second.length());  //iv
            CryptoPP::FileSource("keystore.dat",true,
                                    new CryptoPP::AuthenticatedDecryptionFilter(dec,
                                        new CryptoPP::StringSink(xmlString)));
        }
        QXmlStreamReader reader(QString::fromStdString(xmlString));
        while(!reader.atEnd()){
            if (reader.isStartElement()){
                if (reader.name() == "keyring"){
                    reader.readNext();
                    while(!reader.isStartElement() && !reader.atEnd())
                        reader.readNext();
                    if(reader.name() == "public_key")
                        publicKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                    while(!reader.isStartElement() && !reader.atEnd())
                        reader.readNext();
                    if(reader.name() == "private_key")
                        privateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                }
            }
            reader.readNext();
        }
    }catch(CryptoPP::Exception& err ){
        emit error(QString(err.what()));
    }
    if(validatePrivateKey(privateKey) && validatePublicKey(publicKey)){
        mPrivateKey = privateKey;
        mPublicKey = publicKey;
    }
}
