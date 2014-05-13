#include "rsakeyring.h"

RsaKeyring::RsaKeyring(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    mFileKey(fileKey)
{
    readKeystore();
    connect(&mGenerateWatcher, SIGNAL(finished()),
            this, SLOT(onPrivateKeyGenJobFinished()));
}

void RsaKeyring::updateFileKey(QByteArray *oldKey){
    if(oldKey != NULL){
        std::string xmlString;
        try{
            if(oldKey->isEmpty()){
                QFile keystoreFile("keystore.dat");
                keystoreFile.open(QFile::ReadOnly);
                xmlString = QString(keystoreFile.readAll()).toStdString();
                keystoreFile.close();
            }else{
                CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
                dec.SetKeyWithIV((byte*)oldKey->data(), oldKey->length(),                      //key
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
                            mPrivateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                    }
                }
                reader.readNext();
            }
        }catch(CryptoPP::Exception& err ){
            emit error(QString(err.what()));
        }
    }
    commitToKeystore();
}

void RsaKeyring::commitToKeystore(){
    if(validatePrivateKey(mPrivateKey)){
        QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
        QString xmlString;
        QXmlStreamWriter xml(&xmlString);

        xml.writeStartDocument();
        xml.writeStartElement("keyring");
        xml.writeAttribute("created", timestamp);
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
    }else
        emit error(tr("Failed to validated private key, changes not saved"));
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
    mGenerateWatcher.setFuture(generateFuture);
}

void RsaKeyring::generatePublicKey(){
    std::string pubKeyStr;
    CryptoPP::StringSink pubSink(pubKeyStr);
    CryptoPP::RSA::PrivateKey rsaPrivate;
    try{
        rsaPrivate.Load(CryptoPP::ArraySource((byte*)mPrivateKey.data(), mPrivateKey.size(), true));
        CryptoPP::RSA::PublicKey rsaPublic(rsaPrivate);
        rsaPublic.Save(pubSink);
        mPublicKey.clear();
        mPublicKey.append(pubKeyStr.data(), (int)pubKeyStr.size());
    }catch(CryptoPP::Exception &e){
        qDebug()<<e.what();
        emit error(e.what());
    }
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
    return mGenerateWatcher.isRunning();
}

void RsaKeyring::onPrivateKeyGenJobFinished(){
    mPrivateKey = mGenerateWatcher.result();
    generatePublicKey();
}

void RsaKeyring::setPrivateKey(QByteArray privateKey){
    mPrivateKey = privateKey;
    generatePublicKey();
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
    return QByteArray(privateKey.data(), (int)privateKey.size());
}

bool RsaKeyring::validatePrivateKey(QByteArray privateKey, int level){
    bool success = false;
    CryptoPP::RSA::PrivateKey rsaPrivate;
    CryptoPP::ArraySource source((byte*)privateKey.data(), privateKey.size(), true);
    try{
        rsaPrivate.Load(source);
        success = rsaPrivate.Validate(CryptoPP::AutoSeededRandomPool(), level);
    }catch(CryptoPP::Exception& e){
        emit error(QString(e.what()+tr("level:")+QString::number(level)));
    }
    return success;
}

void RsaKeyring::readKeystore(){
    QByteArray privateKey;
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
                    if(reader.name() == "private_key")
                        privateKey = QByteArray::fromBase64(reader.readElementText().toUtf8());
                }
            }
            reader.readNext();
        }
    }catch(CryptoPP::Exception& err ){
        emit error(QString(err.what()));
    }
    if(validatePrivateKey(privateKey)){
        mPrivateKey = privateKey;
        generatePublicKey();
    }
}
