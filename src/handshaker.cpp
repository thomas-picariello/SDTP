#include "handshaker.h"

Handshaker::Handshaker(TcpLink *link, QPair<QByteArray,QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    m_Link(link),
    m_ContactDB(NULL),
    m_Contact(NULL),
    m_AesDecryptor(new CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption()),
    m_AesEncryptor(new CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption()),
    m_HandshakeFinished(false),
    m_RsaKeyring(fileKey),
    m_MyIntegrityHash(32, 0),       //256 bits Hash
    m_PartnerIntegrityHash(32, 0)   //256 bits Hash
{
    m_SupportedVersions.append(0x01); //(v0.1) TODO:find a better place
    //init rsa decryptor
    try{
        m_RsaDecryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_RsaKeyring.getPrivateKey().data(),
                                                              m_RsaKeyring.getPrivateKey().size(),
                                                              true));
    }catch(CryptoPP::BERDecodeErr& e){
        qDebug()<<"Private key:"<<e.what();
    }
}

void Handshaker::beginResponderHandshake(ContactDB *contactDB){
    m_ContactDB = contactDB;

    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(recieveStarterHello()));

//    emit handshakeFinished(true);
}

void Handshaker::beginStarterHandshake(Contact *contact){
    m_Contact = contact;
    //init rsa encryptor
    try{
        m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                              m_Contact->getKey().size(),
                                                              true));
        sayHello();

    }catch(CryptoPP::BERDecodeErr& e){
        qDebug()<<m_Contact->getName()<<"public key:"<<e.what();
    }



    emit handshakeFinished(true);
}

CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption* Handshaker::getAesEncryptor() const{
    return m_AesEncryptor;
}

CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption* Handshaker::getAesDecryptor() const{
    return m_AesDecryptor;
}

void Handshaker::sayHello(){
    QByteArray packet;
    QByteArray clearText;
    QByteArray cipherText;
    QList<QByteArray*> clearChunks;
    //Security Level
    packet.append((uchar)PreSharedIdentity);
    //Key Length
    QByteArray keyLength;
    keyLength.append((m_RsaKeyring.getPublicKey().size() >> 8) & 0xFF);
    keyLength.append(m_RsaKeyring.getPublicKey().size() & 0xFF);
    clearText.append(keyLength);
    //Public Key
    clearText.append(m_RsaKeyring.getPublicKey());
    //Version List Lenght
    clearText.append(m_SupportedVersions.size());
    //Version List
    clearText.append(m_SupportedVersions);

    //encrypt with Responder public key
    clearChunks = splitData(clearText, (int)m_RsaEncryptor.FixedMaxPlaintextLength());
    foreach(QByteArray *chunk, clearChunks){
        std::string cipherChunk;
        try{
            CryptoPP::ArraySource((byte*)chunk->data(), chunk->size(), true,
                                  new CryptoPP::PK_EncryptorFilter(m_RandomGenerator, m_RsaEncryptor,
                                                                   new CryptoPP::StringSink(cipherChunk)
                                                                   )
                                  );
        }catch(CryptoPP::Exception& e){
            qDebug()<<e.what();
        }
        cipherText.append(cipherChunk.data(), (int)cipherChunk.size());
    }
    qDeleteAll(clearChunks);
    clearChunks.clear();
    packet.append(cipherText);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(recieveResponderHello()));
    qDebug()<<m_Link->write("Hello");
}

void Handshaker::recieveStarterHello(){
    qDebug()<<"Hello recieved!";
    qDebug()<<m_Link->readAll().toHex();
}

void Handshaker::respondHello(){

}

void Handshaker::recieveResponderHello(){
    qDebug()<<"answer recieved";
}

void Handshaker::sendHalfKeyAndPartnerIntegrity(){

}

void Handshaker::recieveHalfKeyAndPartnerIntegrity(){

}

void Handshaker::sendPartnerIntegrity(){

}

void Handshaker::recievePartnerIntegrity(){

}

void Handshaker::sendHandshakeFinished(){

}

void Handshaker::recieveHandshakeFinished(){

}

void Handshaker::sendError(){
    m_Link->write((uchar)UndefinedError);
}

QByteArray Handshaker::genHalfSymKey(){
    const uint blockSize = 32;  //32Bytes = 256 bits
    byte randomBlock[blockSize];
    CryptoPP::AutoSeededRandomPool rnd;
    rnd.GenerateBlock(randomBlock, blockSize);
    return QByteArray((const char*)randomBlock, blockSize);
}

void Handshaker::updateIntegrityHash(QByteArray *currentHash, const QByteArray &data){
    std::string dataToHash;
    dataToHash.append(currentHash->constData(), currentHash->size());
    dataToHash.append(data.constData(), data.size());
    CryptoPP::StringSource(dataToHash, true,
                           new CryptoPP::HashFilter(CryptoPP::SHA256(),
                                                    new CryptoPP::ArraySink((byte*)currentHash->data(), currentHash->size())
                                                    )
                           );
}

bool Handshaker::isError() const{
    return false;
}

QList<QByteArray*> Handshaker::splitData(QByteArray &data, uint chunkSize){
    QList<QByteArray*> chunks;
    for(quint64 offset = 0; offset < (data.size()-chunkSize); offset += chunkSize){
        chunks.append(new QByteArray(data.mid(offset, chunkSize)));
    }
    return chunks;
}
