#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    readKeystore();
    connect(&mWatcher, SIGNAL(finished()),
            this, SLOT(onPrivateKeyGenJobFinished()));
}

void RsaKeyring::changeFileKey(QPair<QByteArray,QByteArray> newKey){
    //TODO: reencrypt keystore.
}

void RsaKeyring::commitToKeystore(){
    if(validatePrivateKey(mPrivateKey) && validatePublicKey(mPublicKey)){
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
    }
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
    QFuture<QByteArray> generateFuture = QtConcurrent::run(this, &RsaKeyring::generatePrivateKeyRunnable);
    mWatcher.setFuture(generateFuture);
}

void RsaKeyring::generatePublicKey(){
    std::string pubKeyStr;
    CryptoPP::StringSink pubSink(pubKeyStr);
    CryptoPP::RSA::PrivateKey rsaPrivate;
    rsaPrivate.Load(CryptoPP::ArraySource((byte*)mPrivateKey.data(), mPrivateKey.size(), true));
    CryptoPP::RSA::PublicKey rsaPublic(rsaPrivate);
    rsaPublic.Save(pubSink);
    mPublicKey.clear();
    mPublicKey.append(pubKeyStr.data(), pubKeyStr.size());
    emit keyGenerationFinished();
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

bool RsaKeyring::isGenerating() const{
    return mWatcher.isRunning();
}

void RsaKeyring::onPrivateKeyGenJobFinished(){
    mPrivateKey = mWatcher.result();
    generatePublicKey();
}

void RsaKeyring::setPrivateKey(QByteArray privateKey){
        mPrivateKey = privateKey;
}

QByteArray RsaKeyring::generatePrivateKeyRunnable(){
    std::string privateKey;
    CryptoPP::StringSink privateSink(privateKey);
    try{
        CryptoPP::RSAES_OAEP_SHA_Decryptor rsaPrivate;
        rsaPrivate.AccessKey().GenerateRandomWithKeySize(CryptoPP::AutoSeededRandomPool(), 4096);
        rsaPrivate.AccessKey().Save(privateSink);
    }catch(CryptoPP::Exception& e){
        emit error(QString(e.what()));
    }
    return QByteArray(privateKey.data(), privateKey.size());
}

bool RsaKeyring::validatePrivateKey(QByteArray privateKey){
    bool success = false;
    CryptoPP::RSA::PrivateKey rsaPrivate;
    CryptoPP::ArraySource source((byte*)privateKey.data(), privateKey.size(), true);
    try{
        rsaPrivate.Load(source);
        success = rsaPrivate.Validate(CryptoPP::AutoSeededRandomPool(), 3);
    }catch(CryptoPP::Exception& e){
       emit error(QString(e.what()));
    }
    return success;
}

bool RsaKeyring::validatePublicKey(QByteArray publicKey){
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
