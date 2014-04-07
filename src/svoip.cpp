#include "svoip.h"

SVoIP::SVoIP(QObject *parent):
    QObject(parent),
    mContactListWindow(&mContactDB, &mFileKey)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    mSalt = settings.value("encryption/salt").toString();
    mPwHash = settings.value("encryption/password_hash").toString();

    if(mSalt.isEmpty()){
        settings.setValue("encryption/salt", QString::fromUtf8(generateSalt()));
    }
    mFileKey.second = QByteArray::fromBase64(mSalt.toUtf8()).left(16); //AES block size

    if(mPwHash.isEmpty()){
        mContactListWindow.show();
    }else{
        connect(&mPasswordWindow, SIGNAL(validate(QString)),
                this, SLOT(onPasswordInput(QString)));
        mPasswordWindow.show();
    }
}

void SVoIP::onPasswordInput(QString password){
    std::string digest;
    CryptoPP::SHA256 hash;
    password.append(QByteArray::fromBase64(mSalt.toUtf8()));
    CryptoPP::StringSource(password.toStdString(),
                           true,
                           new CryptoPP::HashFilter(hash,
                                new CryptoPP::Base64Encoder(
                                    new CryptoPP::StringSink(digest),
                                    false))); //no new line
    qDebug()<<"Hash from entered password"<<QString::fromStdString(digest);
    if(mPwHash.compare(QString::fromStdString(digest)) != 0){
        QMessageBox::critical(&mPasswordWindow, "Error", "Wrong password !");
    }else{
        mPasswordWindow.close();
        mFileKey.first = deriveKey(password);
        mContactListWindow.show();
    }
}

QByteArray SVoIP::deriveKey(QString password){
    CryptoPP::PKCS5_PBKDF1<CryptoPP::SHA256> derivator;
    uchar key[32];
    derivator.DeriveKey(key,
                        derivator.MaxDerivedKeyLength(),
                        0,
                        reinterpret_cast<const uchar*>(password.toUtf8().constData()),
                        password.length(),
                        reinterpret_cast<const uchar*>(mSalt.toUtf8().constData()),
                        mSalt.length(),
                        1000,
                        0);
    qDebug()<<"Gen AES key"
            <<QByteArray(reinterpret_cast<char*>(key), static_cast<uint>(derivator.MaxDerivedKeyLength())).toBase64();
    return QByteArray(reinterpret_cast<char*>(key), static_cast<uint>(derivator.MaxDerivedKeyLength()));
}

QByteArray SVoIP::generateSalt(){
    const unsigned int blockSize = 256;
    byte randomBlock[blockSize];
    std::string encodedBlock;
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(randomBlock,blockSize);
    qDebug()<<randomBlock;
    CryptoPP::ArraySource(randomBlock,
                          blockSize,
                          true,
                          new CryptoPP::Base64Encoder(
                              new StringSink(encodedBlock),
                              false));
    return QString::fromStdString(encodedBlock).toUtf8();
}
