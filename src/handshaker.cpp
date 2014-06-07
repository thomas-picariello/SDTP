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
    m_PartnerIntegrityHash(32, 0)  //256 bits Hash
{
    m_SupportedVersions.append(0x01); //(v0.1) TODO:find a better place
    //init rsa decryptor
    try{
        m_RsaDecryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_RsaKeyring.getPrivateKey().data(),
                                                              m_RsaKeyring.getPrivateKey().size(),
                                                              true));
    }catch(CryptoPP::BERDecodeErr& err){
        emit error(BadPrivateKey);
    }
}

void Handshaker::beginResponderHandshake(ContactDB *contactDB){
    m_ContactDB = contactDB;

    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(parseStarterHello()));
}

void Handshaker::beginStarterHandshake(Contact *contact){
    m_Contact = contact;
    //init rsa encryptor
    try{
        m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                              m_Contact->getKey().size(),
                                                              true));
        sayHello();
    }catch(CryptoPP::BERDecodeErr& err){
        emit error(BadContactKey);
    }
}

CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption* Handshaker::getAesEncryptor() const{
    return m_AesEncryptor;
}

CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption* Handshaker::getAesDecryptor() const{
    return m_AesDecryptor;
}

Contact* Handshaker::getContact() const{
    return m_Contact;
}

void Handshaker::sayHello(){
    QByteArray packet;
    QByteArray clearText;

    //Security Level
    SecurityLevel secLevel = PreSharedIdentity;
    packet.append((uchar)secLevel);

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
    packet.append(rsaEncrypt(clearText));

    //connect response parse methode
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(parseResponderHello()));

    //compute hash
    updateIntegrityHash(&m_MyIntegrityHash, (char)secLevel+clearText);

    //send the packet
    m_Link->write(packet);
}

void Handshaker::parseStarterHello(){
    QByteArray clearText;
    QByteArray rawPacket = m_Link->readAll();

    //parse security level TODO: verify with enum
    byte secLevel = rawPacket.at(0);

    //Decrypt rsa
    QByteArray cipherText = rawPacket.mid(1);
    clearText.append(rsaDecrypt(cipherText));

    //parse Key lenght
    quint16 keyLength = qFromBigEndian<quint16>((const uchar*)clearText.left(2).constData());

    //parse Key
    QByteArray key = clearText.mid(2, keyLength); //pos != index

    //parse Version List length
    byte versionListLenght = clearText.at(2+keyLength);

    //parse Version List
    QByteArray versionList = clearText.right(versionListLenght);

    //check security level
    if(static_cast<SecurityLevel>(secLevel) != PreSharedIdentity){
        emit error(BadSecurityLevel);
        sendError();
        return;
    }

    //check identity
    m_Contact = m_ContactDB->findByKey(key);
    if(m_Contact == NULL){
        emit error(IdentityCheckFailed);
        sendError();
        return;
    }

    //init rsa
    try{
        m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                              m_Contact->getKey().size(),
                                                              true));
    }catch(CryptoPP::BERDecodeErr& err){
        emit error(BadContactKey);
    }

    //check/choose version
    //TODO: check all versions
    byte chosenVersion = 0x01;

    //Compute integrity
    updateIntegrityHash(&m_PartnerIntegrityHash, (char)secLevel+clearText);

    respondHello(chosenVersion);
}

void Handshaker::respondHello(byte chosenVersion){
    //create response packet
    QByteArray packet;
    packet.append("Test Answer...");
    m_Link->write(packet);
}

void Handshaker::parseResponderHello(){
    qDebug()<<"answer recieved:"<<m_Link->readAll();
    emit handshakeFinished(true);
}

void Handshaker::sendHalfKeyAndPartnerIntegrity(){

}

void Handshaker::parseHalfKeyAndPartnerIntegrity(){

}

void Handshaker::sendPartnerIntegrity(){

}

void Handshaker::parsePartnerIntegrity(){

}

void Handshaker::sendHandshakeFinished(){

}

void Handshaker::parseHandshakeFinished(){

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

bool Handshaker::isError() const{
    return false;
}

QByteArray Handshaker::rsaDecrypt(QByteArray& cipherText){
    QByteArray clearText;
    QList<QByteArray*> cipherChunks = splitData(cipherText, (int)m_RsaDecryptor.FixedCiphertextLength());
    foreach(QByteArray *chunk, cipherChunks){
        std::string clearChunk;
        try{
            CryptoPP::ArraySource((byte*)chunk->data(), chunk->size(), true,
                                  new CryptoPP::PK_DecryptorFilter(m_RandomGenerator, m_RsaDecryptor,
                                                                   new CryptoPP::StringSink(clearChunk)
                                                                   )
                                  );
            clearText.append(clearChunk.data(), (int)clearChunk.size());
        }catch(CryptoPP::Exception& e){
            qDebug()<<e.what();
            emit error(DataCorrupted);
            clearText.clear();
            return clearText;
        }
    }
    qDeleteAll(cipherChunks);
    cipherChunks.clear();
    return clearText;
}

QByteArray Handshaker::rsaEncrypt(QByteArray& clearText){
    QByteArray cipherText;
    QList<QByteArray*> clearChunks = splitData(clearText, (int)m_RsaEncryptor.FixedMaxPlaintextLength());
    foreach(QByteArray *chunk, clearChunks){
        std::string cipherChunk;
        try{
            CryptoPP::ArraySource((byte*)chunk->data(), chunk->size(), true,
                                  new CryptoPP::PK_EncryptorFilter(m_RandomGenerator, m_RsaEncryptor,
                                                                   new CryptoPP::StringSink(cipherChunk)
                                                                   )
                                  );
            cipherText.append(cipherChunk.data(), (int)cipherChunk.size());
        }catch(CryptoPP::Exception& e){
            qDebug()<<e.what();
            cipherText.clear();

        }
    }
    qDeleteAll(clearChunks);
    clearChunks.clear();
    return cipherText;
}

QList<QByteArray*> Handshaker::splitData(QByteArray &data, uint chunkSize){
    QList<QByteArray*> chunks;
    quint64 offset = 0;
    do{
        chunks.append(new QByteArray(data.mid(offset, chunkSize)));
        offset += chunkSize;
    }while(offset < data.size());
    return chunks;
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
