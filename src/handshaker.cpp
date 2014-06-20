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
    m_Mode(UndefinedMode),
    m_BanTime(0)
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

quint16 Handshaker::getBanTime() const{
    return m_BanTime;
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
    m_BanTime = m_IpFilter->getLastBanTime(m_Link->getHost().first);
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
            //init rsa decryptor
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
            //init rsa encryptor
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

void Handshaker::starterSayHello(){ //S:1
    QByteArray packet;
    QByteArray clearText;

    //Security Level
    SecurityLevel secLevel = PreSharedIdentity;
    packet.append((uchar)secLevel);

    //Key Length
    QByteArray keyLength;
    QByteArray publicKey = m_RsaKeyring->generatePublicKey();
    keyLength.append((publicKey.size() >> 8) & 0xFF);
    keyLength.append(publicKey.size() & 0xFF);
    clearText.append(keyLength);

    //Public Key
    clearText.append(publicKey);

    //Version
    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    //compute starter integrity
    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);

    //encrypt with Responder public key
    packet.append(rsaEncrypt(clearText));

    //connect response parse methode
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(starterParseResponderHello()));

    //send the packet
    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::responderParseStarterHello(){ //R:1.1
    QByteArray clearText;
    QByteArray rawPacket = m_Link->readAll();

    //check error
    if(isError(rawPacket)) return;

    //parse security level
    byte secLevel = rawPacket.at(0);

    //Decrypt rsa
    QByteArray cipherText = rawPacket.mid(1);
    clearText.append(rsaDecrypt(cipherText));
    if(clearText.size() == 0){
        processError(DataCorrupted);
        return;
    }

    //parse Key lenght
    quint16 keyLength = qFromBigEndian<quint16>((const uchar*)clearText.left(2).constData());
    //parse Key
    QByteArray key = clearText.mid(2, keyLength); //pos != index

    //parse Version
    byte version = clearText.at(clearText.size()-1);

    //check security level
    if(static_cast<SecurityLevel>(secLevel) != PreSharedIdentity){
        processError(BadSecurityLevel);
        return;
    }

    //check identity
    m_Contact = m_ContactDB->findByKey(key);
    if(m_Contact == NULL){
        processError(IdentityCheckFailed);
        return;
    }
    emit newContactId(m_Contact->getId());

    //init rsa
    try{
        m_RsaEncryptor.AccessKey().Load(CryptoPP::ArraySource((byte*)m_Contact->getKey().data(),
                                                              m_Contact->getKey().size(),
                                                              true));
    }catch(CryptoPP::BERDecodeErr&){
        processError(BadContactKey);
    }

    //check version compatibility (major version only)
    if((version & 0xF0) != (SUPPORTED_PROTOCOL_VERSION & 0xF0)){
        processError(IncompatibleProtocolVersions);
        return;
    }

    //update starter integrity
    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);

    responderRespondHello();
}

void Handshaker::responderRespondHello(){ //R:1.2
    QByteArray packet, clearText;

    //chosen version
    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    //half symmetric key + half IV
    QByteArray firstHalfSymKey = generateRandomBlock(32);
    m_GcmKey.first.append(firstHalfSymKey.left(16));
    m_GcmKey.second.append(firstHalfSymKey.right(16));
    clearText.append(firstHalfSymKey);

    //update responder integrity
    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);

    //encrypt and add to packet
    packet.append(rsaEncrypt(clearText));

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(responderParseHalfKeyAndResponderIntegrity()));

    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::starterParseResponderHello(){ //S:2.1
    QByteArray rawPacket = m_Link->readAll();
    m_Timeout.stop();

    //check error
    if(isError(rawPacket)) return;

    //decrypt packet
    QByteArray clearText = rsaDecrypt(rawPacket);
    if(clearText.isEmpty()){
        processError(DataCorrupted);
        return;
    }

    //parse chosen version
    byte chosenVersion = clearText.at(0);

    //parse first half key + half IV
    m_GcmKey.first.append(clearText.mid(1, 16));
    m_GcmKey.second.append(clearText.mid(17));

    //check version
    //TODO: proper check
    if(chosenVersion != 0x01){
        processError(IncompatibleProtocolVersions);
        return;
    }

    //check half key + half IV size
    if(m_GcmKey.first.size()!=16 || m_GcmKey.second.size()!=16){
        processError(BadSymmetricKey);
        return;
    }

    //update responder integrity
    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);

    starterSendHalfKeyAndResponderIntegrity();
}

void Handshaker::starterSendHalfKeyAndResponderIntegrity(){ //S:2.2
    QByteArray packet;

    //generate second half key+IV
    QByteArray secondHalfSymKey = generateRandomBlock(32);
    m_GcmKey.first.append(secondHalfSymKey.left(16));    //key
    m_GcmKey.second.append(secondHalfSymKey.right(16));  //IV

    //rsa encrypt half key+IV
    QByteArray encryptedHalfKey = rsaEncrypt(secondHalfSymKey);
    packet.append(static_cast<char>(encryptedHalfKey.size()>>8 & 0xFF));
    packet.append(static_cast<char>(encryptedHalfKey.size() & 0xFF));
    packet.append(encryptedHalfKey);

    //init GCM
    m_GcmDecryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());
    m_GcmEncryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size()); 

    //encrypt responder integrity with gcm
    packet.append(gcmEncrypt(m_ResponderIntegrityHash));

    //update starter integrity
    updateIntegrityHash(&m_StarterIntegrityHash, secondHalfSymKey+m_ResponderIntegrityHash);

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(starterParseStarterIntegrity()));

    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::responderParseHalfKeyAndResponderIntegrity(){ //R:2.1
    QByteArray rawPacket = m_Link->readAll();
    m_Timeout.stop();

    //check error
    if(isError(rawPacket)) return;

    //parse encrypted key lenght
    quint16 encryptedKeyLenght = (rawPacket.at(0) << 8) + rawPacket.at(1);

    //parse encrypted key
    QByteArray encryptedSymKey = rawPacket.mid(2,encryptedKeyLenght);

    //decrypt key
    QByteArray clearSymKey = rsaDecrypt(encryptedSymKey);

    //check key lenght
    if(clearSymKey.size()<32){
        processError(BadSymmetricKey);
        return;
    }

    //init gcm
    m_GcmKey.first.append(clearSymKey.left(16));    //key
    m_GcmKey.second.append(clearSymKey.right(16));  //IV

    m_GcmDecryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());
    m_GcmEncryptor->SetKeyWithIV((byte*)m_GcmKey.first.data(), m_GcmKey.first.size(),
                                 (byte*)m_GcmKey.second.data(), m_GcmKey.second.size());


    //parse responder integrity
    QByteArray encryptedResponderIntegrity = rawPacket.mid(2+encryptedKeyLenght);

    //decrypt integrity
    QByteArray responderIntegrity = gcmDecrypt(encryptedResponderIntegrity);

    //check integrity
    if(responderIntegrity != m_ResponderIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    //update starter integrity
    updateIntegrityHash(&m_StarterIntegrityHash, clearSymKey+responderIntegrity);

    responderSendStarterIntegrity();
}

void Handshaker::responderSendStarterIntegrity(){ //R:2.2
    QByteArray packet;

    //encrypt starter integrity
    packet.append(gcmEncrypt(m_StarterIntegrityHash));

    //connect incomming data to responderParseHandshakeFinished()
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    connect(m_Link, SIGNAL(readyRead()),
            this, SLOT(responderParseHandshakeFinished()));

    //send packet
    m_Link->write(packet);
    m_Timeout.start();
}

void Handshaker::starterParseStarterIntegrity(){ //S:3.1
    QByteArray rawPacket = m_Link->readAll();
    m_Timeout.stop();

    //check error
    if(isError(rawPacket)) return;

    //decrypt packet
    QByteArray starterIntegrity = gcmDecrypt(rawPacket);

    //check integrity
    if(starterIntegrity != m_StarterIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    starterSendHandshakeFinished();
}

void Handshaker::starterSendHandshakeFinished(){ //S:3.2
    QByteArray packet;
    packet.append((char)HandshakeFinished);

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    m_Link->write(packet);

    emit handshakeFinished(true);
}

void Handshaker::responderParseHandshakeFinished(){ //R:3
    QByteArray rawPacket = m_Link->readAll();
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    m_Timeout.stop();

    //check error
    if(isError(rawPacket))
        return;
    else if(rawPacket.size() == 1 && rawPacket.at(0) == (char)HandshakeFinished){
        emit handshakeFinished(true);
    }else{
        processError(UndefinedError);
    }
}

void Handshaker::onTimeout(){
    processError(Timeout);
}

void Handshaker::processError(Error err){
    QByteArray packet;
    QDataStream packetizer(&packet, QIODevice::ReadWrite);
    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    if(m_BanTime > 0){
        if(m_BanTime < 65535)
            m_BanTime *= 2;
    }else
        m_BanTime = 1;
    packetizer << (byte)UndefinedError;
    packetizer << m_BanTime;
    m_Link->write(packet);
    m_IpFilter->addBan(m_Link->getHost().first, m_BanTime);
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

bool Handshaker::isError(const QByteArray &data){
    QDataStream depacketizer(data);
    byte errorCode;
    depacketizer >> errorCode;
    if(data.size() == 3 && errorCode == (byte)UndefinedError){
        disconnect(m_Link, SIGNAL(readyRead()), this, 0);
        depacketizer >> m_BanTime;
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
