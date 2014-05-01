#include "svoip.h"

SVoIP::SVoIP(QObject *parent):
    QObject(parent),
    mContactDB(NULL),
    mContactListWindow(NULL),
    mPasswordWindow(NULL)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    QString salt = settings.value("encryption/salt").toString();
    QString pwdHash = settings.value("encryption/password_hash").toString();

    if(salt.isEmpty()){
        settings.setValue("encryption/salt", generateSalt());
    }
    //set IV (AES block size = 128 bits)
    mFileKey.second = QByteArray::fromBase64(salt.toUtf8()).left(16);

    if(pwdHash.isEmpty()){
        startProgram();
    }else{
        mPasswordWindow = new PasswordWindow(pwdHash, salt);
        connect(mPasswordWindow, SIGNAL(validate(QByteArray)),
                this, SLOT(startProgram(QByteArray)));
    }
}

void SVoIP::startProgram(QByteArray key){
    mFileKey.first = key;
    mContactDB = new ContactDB(&mFileKey, this);
    mContactListWindow = new ContactListWindow(mContactDB, &mFileKey);
}

QString SVoIP::generateSalt(){
    const unsigned int blockSize = 32;
    byte randomBlock[blockSize];
    std::string encodedBlock;
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(randomBlock,blockSize);
    CryptoPP::ArraySource(randomBlock,
                          blockSize,
                          true,
                          new CryptoPP::Base64Encoder(
                              new StringSink(encodedBlock),
                              false));
    return QString::fromStdString(encodedBlock);
}

SVoIP::~SVoIP(){
    if(mContactDB) delete mContactDB;
    if(mContactListWindow) delete mContactListWindow;
    if(mPasswordWindow) delete mPasswordWindow;
}
