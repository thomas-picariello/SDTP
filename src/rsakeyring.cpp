#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    readKeystore();
    connect(&mGenerateWatcher, SIGNAL(finished()),
            this, SLOT(onPrivateKeyGenJobFinished()));
}

void RsaKeyring::updateFileKey(QByteArray &oldKey){
    std::string xmlString;
    try{
        if(oldKey.isEmpty()){
            QFile keystoreFile("keystore.dat");
            keystoreFile.open(QFile::ReadOnly);
            xmlString = QString(keystoreFile.readAll()).toStdString();
            keystoreFile.close();
        }else{
            CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
            dec.SetKeyWithIV((byte*)oldKey.data(), oldKey.length(),                      //key
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
                    if(reader.name() == "private_key")
                        mStoredPrivateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                }
            }
            reader.readNext();
        }
    }catch(CryptoPP::Exception&){
        emit error(KeystoreLoadingFailed);
    }
    commitToKeystore(mStoredPrivateKey);
}

void RsaKeyring::commitToKeystore(QByteArray privateKey){
    if(validatePrivateKey(privateKey)){
        mStoredPrivateKey = privateKey;
        QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
        QString xmlString;
        QXmlStreamWriter xml(&xmlString);

        xml.writeStartDocument();
        xml.writeStartElement("keyring");
        xml.writeAttribute("created", timestamp);
        xml.writeTextElement("private_key", privateKey.toBase64());
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
    }
}

void RsaKeyring::generateKeypair(){
    QFuture<QByteArray> generateFuture = QtConcurrent::run(this, &RsaKeyring::generatePrivateKeyRunnable);
    mGenerateWatcher.setFuture(generateFuture);
}

QByteArray RsaKeyring::generatePublicKey(QByteArray &privateKey){
    std::string pubKeyStr;
    CryptoPP::StringSink pubSink(pubKeyStr);
    CryptoPP::RSA::PrivateKey rsaPrivate;
    try{
        if(privateKey.isEmpty())
            rsaPrivate.Load(CryptoPP::ArraySource((byte*)mStoredPrivateKey.data(), mStoredPrivateKey.size(), true));
        else
            rsaPrivate.Load(CryptoPP::ArraySource((byte*)privateKey.data(), privateKey.size(), true));
        CryptoPP::RSA::PublicKey rsaPublic(rsaPrivate);
        rsaPublic.Save(pubSink);
    }catch(CryptoPP::Exception& e){
        //qDebug()<<e.what();
        emit error(PublicKeyGenerationFailed);
    }
    QByteArray publicKey(pubKeyStr.data(), (int)pubKeyStr.size());
    emit publicKeyGenerationFinished(publicKey);
    return publicKey;
}

QByteArray RsaKeyring::getStoredPrivateKey() const{
    return mStoredPrivateKey;
}

bool RsaKeyring::isGenerating() const{
    return mGenerateWatcher.isRunning();
}

bool RsaKeyring::hasPrivateKey() const{
    return !mStoredPrivateKey.isEmpty();
}

void RsaKeyring::onPrivateKeyGenJobFinished(){
    QByteArray privateKey = mGenerateWatcher.result();
    emit privateKeyGenerationFinished(privateKey);
    generatePublicKey(privateKey);
}

QByteArray RsaKeyring::generatePrivateKeyRunnable(){
    std::string privateKey;
    CryptoPP::StringSink privateSink(privateKey);
    try{
        CryptoPP::RSAES_OAEP_SHA_Decryptor rsaPrivate;
        rsaPrivate.AccessKey().GenerateRandomWithKeySize(CryptoPP::AutoSeededRandomPool(), 4096);
        rsaPrivate.AccessKey().Save(privateSink);
    }catch(CryptoPP::Exception&){
        emit error(PrivatKeyGenerationFailed);
    }
    return QByteArray(privateKey.data(), (int)privateKey.size());
}

bool RsaKeyring::validatePrivateKey(QByteArray privateKey, int level){
    bool success = false;
    CryptoPP::RSA::PrivateKey rsaPrivate;
    CryptoPP::ArraySource source((byte*)privateKey.data(), privateKey.size(), true);
    try{
        rsaPrivate.Load(source);
        success = rsaPrivate.Validate(CryptoPP::AutoSeededRandomPool(), level);
    }catch(CryptoPP::Exception&){
        emit error(PrivateKeyValidationFailed);
    }
    if(success)
        emit privateKeyValidated();
    return success;
}

void RsaKeyring::readKeystore(){
    QByteArray privateKey;
    std::string xmlString;
    if(!QFile("keystore.dat").exists()){
        emit error(KeystoreFileNotFound);
        return;
    }
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
                    if(reader.name() == "private_key")
                        privateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                }
            }
            reader.readNext();
        }
    }catch(CryptoPP::Exception& e){
        //qDebug()<<e.what();
        emit error(KeystoreLoadingFailed);
    }
    if(validatePrivateKey(privateKey)){
        mStoredPrivateKey = privateKey;
        generatePublicKey(privateKey);
    }
}
