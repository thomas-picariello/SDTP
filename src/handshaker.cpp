#include "handshaker.h"

Handshaker::Handshaker(TcpLink *link, RsaKeyring* keyring, QObject *parent):
    QObject(parent),
    m_Link(link),
    m_ContactDB(NULL),
    m_Contact(NULL),
    m_IpFilter(NULL),
    m_GcmDecryptor(new CryptoPP::GCM<CryptoPP::AES>::Decryption()),
    m_GcmEncryptor(new CryptoPP::GCM<CryptoPP::AES>::Encryption()),
    m_RsaKeyring(keyring),
    m_StarterIntegrityHash(32, 0),  //256 bits Hash
    m_ResponderIntegrityHash(32, 0),  //256 bits Hash
    m_Mode(UndefinedMode)
{
    m_Timeout.setSingleShot(true);
    m_Timeout.setInterval(5000);    //default Timeout 5s
    resetHandshake();

    connect(&m_Timeout, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
}

void Handshaker::waitForHandshake(ContactDB *contactDB){
    m_Mode = ResponderMode;
    m_ContactDB = contactDB;
    if(m_RsaKeyring->hasPrivateKey()){ //security check
        resetHandshake();
    }else{
        emit error(BadPrivateKey);
        connect(m_RsaKeyring, SIGNAL(privateKeyValidated()),
                this, SLOT(resetHandshake()));
    }
}

void Handshaker::beginStarterHandshake(Contact *contact){
    m_Mode = StarterMode;
    m_Contact = contact;
    if(m_RsaKeyring->hasPrivateKey()){ //security check
        resetHandshake();
    }else{
        emit error(BadPrivateKey);
        connect(m_RsaKeyring, SIGNAL(privateKeyValidated()),
                this, SLOT(resetHandshake()));
    }
}

qint32 Handshaker::getBanTime() const{
    return m_IpFilter->getLastBanTime(m_Link->getHost());
}

Contact* Handshaker::getContact() const{
    return m_Contact;
}

QString Handshaker::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
}

CryptoPP::GCM<CryptoPP::AES>::Encryption* Handshaker::getGcmEncryptor() const{
    return m_GcmEncryptor;
}

CryptoPP::GCM<CryptoPP::AES>::Decryption* Handshaker::getGcmDecryptor() const{
    return m_GcmDecryptor;
}

Handshaker::Mode Handshaker::getMode() const{
    return m_Mode;
}

void Handshaker::setIpFilter(IpFilter *ipFilter){
    m_IpFilter = ipFilter;
}

void Handshaker::setTimeout(int timeout){
    m_Timeout.setInterval(timeout);
}

void Handshaker::resetHandshake(){
    disconnect(m_RsaKeyring, SIGNAL(privateKeyValidated()), this, 0);
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    m_StarterIntegrityHash.fill(32, 0);
    m_ResponderIntegrityHash.fill(32, 0);

    switch(m_Mode){
    case UndefinedMode:
        try{
            m_RsaDecryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_RsaKeyring->getStoredPrivateKey().data(),
                                                                  m_RsaKeyring->getStoredPrivateKey().size(),
                                                                  true));
        }catch(CryptoPP::BERDecodeErr&){
            emit error(BadPrivateKey);
            connect(m_RsaKeyring, SIGNAL(privateKeyValidated()),
                    this, SLOT(resetHandshake()));
        }
        break;
    case StarterMode:
        try{
            m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                                  m_Contact->getKey().size(),
                                                                  true));
            starterSayHello();
        }catch(CryptoPP::BERDecodeErr&){
            emit error(BadContactKey);
            connect(m_RsaKeyring, SIGNAL(privateKeyValidated()),
                    this, SLOT(resetHandshake()));
        }
        break;
    case ResponderMode:
        connect(m_Link, SIGNAL(readyRead()),
                this, SLOT(responderParseStarterHello()));
        break;
    }
}

void Handshaker::starterSayHello(){ //S:1 forge
    QByteArray packet;
    QDataStream packetStream(&packet, QIODevice::WriteOnly);
    QByteArray clearText;
    QDataStream clearTextStream(&clearText, QIODevice::WriteOnly);

    SecurityLevel secLevel = PreSharedIdentity;
    packetStream << (quint8)secLevel;

    QByteArray publicKey = m_RsaKeyring->generatePublicKey();
    clearTextStream << (quint16)publicKey.size();
    clearTextStream.writeRawData(publicKey.data(), publicKey.size());

    clearTextStream << (quint8)SUPPORTED_PROTOCOL_VERSION;

    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);

    packet.append(rsaEncrypt(clearText));

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(starterParseResponderHello()));

    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::responderParseStarterHello(){ //R:1 parse
    QByteArray packet = m_Link->readAll();

    if(isError(packet)) return;

    quint8 secLevel = packet.at(0);
    if(static_cast<SecurityLevel>(secLevel) != PreSharedIdentity){
        processError(BadSecurityLevel);
        return;
    }

    QByteArray clearText = rsaDecrypt(packet.mid(1));
    if(clearText.isEmpty()){
        processError(DataCorrupted);
        return;
    }
    quint16 keyLength = qFromBigEndian<quint16>((const uchar*)clearText.left(2).constData());
    QByteArray key = clearText.mid(2, keyLength);
    m_Contact = m_ContactDB->findByKey(key);
    if(m_Contact == NULL){
        processError(IdentityCheckFailed);
        return;
    }
    emit newContactId(m_Contact->getId());
    try{
        m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                              m_Contact->getKey().size(),
                                                              true));
    }catch(CryptoPP::BERDecodeErr&){
        processError(BadContactKey);
    }

    byte version = clearText.right(1).at(0);
    if((version & 0xF0) != (SUPPORTED_PROTOCOL_VERSION & 0xF0)){
        processError(IncompatibleProtocolVersions);
        return;
    }

    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);
    responderRespondHello();
}

void Handshaker::responderRespondHello(){ //R:1 forge
    QByteArray packet, clearText;

    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    QByteArray firstHalfSymKey = generateRandomBlock(32);
    m_GcmKey.first.swap(firstHalfSymKey.left(16));  //first half key
    m_GcmKey.second.swap(firstHalfSymKey.right(16));//first half IV
    clearText.append(firstHalfSymKey);

    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);

    packet.append(rsaEncrypt(clearText));

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(responderParseHalfKeyAndResponderIntegrity()));
    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::starterParseResponderHello(){ //S:2 parse
    m_Timeout.stop();
    QByteArray packet = m_Link->readAll();

    if(isError(packet)) return;

    QByteArray clearText = rsaDecrypt(packet);
    if(clearText.isEmpty()){
        processError(DataCorrupted);
        return;
    }

    byte chosenVersion = clearText.at(0);
    if((chosenVersion & 0xF0) != (SUPPORTED_PROTOCOL_VERSION & 0xF0)){
        processError(IncompatibleProtocolVersions);
        return;
    }

    m_GcmKey.first.append(clearText.mid(1, 16));//first half key
    m_GcmKey.second.append(clearText.mid(17));  //first half IV
    if(m_GcmKey.first.size()!=16 || m_GcmKey.second.size()!=16){
        processError(BadSymmetricKey);
        return;
    }

    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);
    starterSendHalfKeyAndResponderIntegrity();
}

void Handshaker::starterSendHalfKeyAndResponderIntegrity(){ //S:2 forge
    QByteArray packet;

    QByteArray secondHalfSymKey = generateRandomBlock(32);
    m_GcmKey.first.append(secondHalfSymKey.left(16));    //second half key
    m_GcmKey.second.append(secondHalfSymKey.right(16));  //second half IV

    QByteArray encryptedHalfKey = rsaEncrypt(secondHalfSymKey);
    QDataStream(&packet, QIODevice::WriteOnly) << (quint16)encryptedHalfKey.size();
    packet.append(encryptedHalfKey);

    m_GcmDecryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());
    m_GcmEncryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size()); 

    packet.append(gcmEncrypt(m_ResponderIntegrityHash));

    updateIntegrityHash(&m_StarterIntegrityHash, secondHalfSymKey+m_ResponderIntegrityHash);

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(starterParseStarterIntegrity()));

    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::responderParseHalfKeyAndResponderIntegrity(){ //R:2 parse
    m_Timeout.stop();
    QByteArray packet = m_Link->readAll();

    if(isError(packet)) return;
    quint16 encryptedKeyLenght = qFromBigEndian<quint16>((const uchar*)packet.left(2).constData());

    QByteArray clearSymKey = rsaDecrypt(packet.mid(2,encryptedKeyLenght));
    if(clearSymKey.size()<32){
        processError(BadSymmetricKey);
        return;
    }

    m_GcmKey.first.append(clearSymKey.left(16));    //second half key
    m_GcmKey.second.append(clearSymKey.right(16));  //second half IV

    m_GcmDecryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());
    m_GcmEncryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());

    QByteArray responderIntegrity = gcmDecrypt(packet.mid(2+encryptedKeyLenght));
    if(responderIntegrity != m_ResponderIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    updateIntegrityHash(&m_StarterIntegrityHash, clearSymKey+responderIntegrity);
    responderSendStarterIntegrity();
}

void Handshaker::responderSendStarterIntegrity(){ //R:2 forge
    QByteArray packet;
    packet.append(gcmEncrypt(m_StarterIntegrityHash));

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(responderParseHandshakeFinished()));

    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::starterParseStarterIntegrity(){ //S:3 parse
    m_Timeout.stop();
    QByteArray rawPacket = m_Link->readAll();

    if(isError(rawPacket)) return;

    QByteArray starterIntegrity = gcmDecrypt(rawPacket);
    if(starterIntegrity != m_StarterIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    starterSendHandshakeFinished();
}

void Handshaker::starterSendHandshakeFinished(){ //S:3 forge
    QByteArray packet;
    packet.append((char)HandshakeFinished);

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    m_Link->write(packet);

    m_IpFilter->removeBan(m_Link->getHost());
    emit handshakeFinished(true);
}

void Handshaker::responderParseHandshakeFinished(){ //R:3 parse
    m_Timeout.stop();
    QByteArray rawPacket = m_Link->readAll();
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);

    if(isError(rawPacket))
        return;
    else if(rawPacket.size() == 1 && rawPacket.at(0) == (char)HandshakeFinished){
        m_IpFilter->removeBan(m_Link->getHost());
        emit handshakeFinished(true);
    }else{
        processError(UndefinedError);
    }
}

void Handshaker::onTimeout(){
    processError(Timeout);
}

void Handshaker::processError(Error err){
    quint16 banTime = getBanTime(); //implicite cast
    QByteArray packet;
    QDataStream packetStream(&packet, QIODevice::WriteOnly);
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    if(banTime > 0){
        if(banTime <= 32767)
            banTime *= 2;
    }else
        banTime = 1;
    packetStream << (quint8)UndefinedError;
    packetStream << banTime;
    m_Link->write(packet);
    m_IpFilter->addBan(m_Link->getHost(), banTime);
    emit error(err);
    emit handshakeFinished(false);
}

QByteArray Handshaker::generateRandomBlock(uint size){
    QByteArray randomBlock;
    randomBlock.resize(size);
    CryptoPP::AutoSeededRandomPool rnd;
    rnd.GenerateBlock((byte*)randomBlock.data(), size);
    return randomBlock;
}

QByteArray Handshaker::gcmDecrypt(QByteArray& cipherText){
    std::string clearText;
    try{
        CryptoPP::ArraySource((byte*)cipherText.data(), cipherText.size(), true,
                              new CryptoPP::AuthenticatedDecryptionFilter(*m_GcmDecryptor,
                                                                          new CryptoPP::StringSink(clearText)
                                                                          )
                              );
    }catch(CryptoPP::Exception&){
        emit error(DataCorrupted);
    }
    return QByteArray(clearText.data(), (int)clearText.size());
}

QByteArray Handshaker::gcmEncrypt(QByteArray& clearText){
    std::string cipherText;
    try{
        CryptoPP::ArraySource((byte*)clearText.data(), clearText.size(), true,
                              new CryptoPP::AuthenticatedEncryptionFilter(*m_GcmEncryptor,
                                                                          new CryptoPP::StringSink(cipherText)
                                                                          )
                              );
    }catch(CryptoPP::Exception&){
        emit error(BadSymmetricKey);
    }
    return QByteArray(cipherText.data(), (int)cipherText.size());
}

bool Handshaker::isError(const QByteArray &packet){
    QDataStream packetStream(packet);
    quint8 errorCode;
    packetStream >> errorCode;
    if(packet.size() == 3 && errorCode == (quint8)UndefinedError){
        disconnect(m_Link, SIGNAL(readyRead()), this, 0);
        quint16 banTime = getBanTime();
        packetStream >> banTime;
        handshakeFinished(false);
        emit error(UndefinedError);
        return true;
    }
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
                                                                   new CryptoPP::StringSink(clearChunk)));

            clearText.append(clearChunk.data(), (int)clearChunk.size());
        }catch(CryptoPP::Exception&){
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
        }catch(CryptoPP::Exception&){
            //TODO: process error ?
            cipherText.clear();
        }
    }
    qDeleteAll(clearChunks);
    clearChunks.clear();
    return cipherText;
}

QList<QByteArray*> Handshaker::splitData(const QByteArray &data, const uint chunkSize){
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
