#include "handshaker.h"

Handshaker::Handshaker(TcpLink *link, RsaKeyring* keyring, QObject *parent):
    QObject(parent),
    m_Link(link),
    m_LinkStream(m_Link),
    m_ContactDB(NULL),
    m_Contact(NULL),
    m_IpFilter(NULL),
    m_RsaKeyring(keyring),
    m_StarterIntegrityHash(32, 0),  //256 bits Hash
    m_ResponderIntegrityHash(32, 0),  //256 bits Hash
    m_Mode(UndefinedMode),
    m_IvOffset(0),
    m_RecievedBanTime(0)
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

quint16 Handshaker::getRecievedBanTime() const{
    return m_RecievedBanTime;
}

Contact* Handshaker::getContact() const{
    return m_Contact;
}

QString Handshaker::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
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
    m_RecievedBanTime = 0;

    switch(m_Mode){
    case UndefinedMode:
        try{
            m_RsaDecryptor.AccessKey().Load(ArraySource((byte*)m_RsaKeyring->getStoredPrivateKey().data(),
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
            m_RsaEncryptor.AccessKey().Load(ArraySource((byte*)m_Contact->getKey().data(),
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

    clearTextStream << m_RsaKeyring->generatePublicKey();

    clearTextStream << SUPPORTED_PROTOCOL_VERSION;

    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);

    packetStream << rsaEncrypt(clearText);

    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    connect(m_Link, &AbstractLink::readyRead,
            this, &Handshaker::starterParseResponderHello);

    m_LinkStream << packet;
    m_Timeout.start();
}

void Handshaker::responderParseStarterHello(){ //R:1 parse
    QByteArray packet;
    m_LinkStream >> packet;
    QDataStream packetStream(&packet, QIODevice::ReadOnly);

    if(isError(packet)) return;

    quint8 secLevel;
    packetStream >> secLevel;
    if(static_cast<SecurityLevel>(secLevel) != PreSharedIdentity){
        processError(BadSecurityLevel);
        return;
    }

    QByteArray rsaCypherText;
    packetStream >> rsaCypherText;
    QByteArray clearText = rsaDecrypt(rsaCypherText);
    if(clearText.isEmpty()){
        processError(DataCorrupted);
        return;
    }
    QDataStream clearTextStream(&clearText, QIODevice::ReadOnly);
    QByteArray key;
    clearTextStream >> key;
    m_Contact = m_ContactDB->findByKey(key);
    if(m_Contact == NULL){
        processError(IdentityCheckFailed);
        return;
    }
    emit newContactId(m_Contact->getId());
    try{
        m_RsaEncryptor.AccessKey().Load(ArraySource((byte*)m_Contact->getKey().data(),
                                                    m_Contact->getKey().size(),
                                                    true));
    }catch(CryptoPP::BERDecodeErr&){
        processError(BadContactKey);
    }

    quint8 version;
    clearTextStream >> version;
    if((version & 0xF0) != (SUPPORTED_PROTOCOL_VERSION & 0xF0)){
        processError(IncompatibleProtocolVersions);
        return;
    }

    updateIntegrityHash(&m_StarterIntegrityHash, (char)secLevel+clearText);
    responderRespondHello();
}

void Handshaker::responderRespondHello(){ //R:1 forge
    QByteArray clearText;

    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    QByteArray firstHalfSymKey = generateRandomBlock(32);
    m_GcmKey.append(firstHalfSymKey.left(16));  //first half key
    m_GcmBaseIV.append(firstHalfSymKey.right(16));//first half IV
    clearText.append(firstHalfSymKey);

    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);

    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    connect(m_Link, &AbstractLink::readyRead,
            this, &Handshaker::responderParseHalfKeyAndResponderIntegrity);

    m_LinkStream << rsaEncrypt(clearText);
    m_Timeout.start();
}

void Handshaker::starterParseResponderHello(){ //S:2 parse
    m_Timeout.stop();
    QByteArray rsaCypherText, clearText;
    m_LinkStream >> rsaCypherText;
    if(isError(rsaCypherText)) return;

    clearText = rsaDecrypt(rsaCypherText);

    if(clearText.isEmpty()){
        processError(DataCorrupted);
        return;
    }

    quint8 chosenVersion = clearText.at(0);
    if((chosenVersion & 0xF0) != (SUPPORTED_PROTOCOL_VERSION & 0xF0)){
        processError(IncompatibleProtocolVersions);
        return;
    }

    m_GcmKey.append(clearText.mid(1, 16));  //first half key
    m_GcmBaseIV.append(clearText.mid(1 + 16));  //first half IV
    if(m_GcmKey.size()!=16 || m_GcmBaseIV.size()!=16){
        processError(BadSymmetricKey);
        return;
    }

    updateIntegrityHash(&m_ResponderIntegrityHash, clearText);
    starterSendHalfKeyAndResponderIntegrity();
}

void Handshaker::starterSendHalfKeyAndResponderIntegrity(){ //S:2 forge
    QByteArray secondHalfSymKey = generateRandomBlock(32);
    m_GcmKey.append(secondHalfSymKey.left(16));     //second half key
    m_GcmBaseIV.append(secondHalfSymKey.right(16)); //second half IV

    updateIntegrityHash(&m_StarterIntegrityHash, secondHalfSymKey+m_ResponderIntegrityHash);

    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    connect(m_Link, &AbstractLink::readyRead,
            this, &Handshaker::starterParseStarterIntegrity);

    m_LinkStream << rsaEncrypt(secondHalfSymKey);
    m_LinkStream << gcmEncrypt(m_ResponderIntegrityHash);

    m_Timeout.start();
}

void Handshaker::responderParseHalfKeyAndResponderIntegrity(){ //R:2 parse
    m_Timeout.stop();
    QByteArray encryptedSymKey;
    m_LinkStream >> encryptedSymKey;
    if(isError(encryptedSymKey)) return;

    QByteArray clearSymKey = rsaDecrypt(encryptedSymKey);
    if(clearSymKey.size()<32){
        processError(BadSymmetricKey);
        return;
    }
    m_GcmKey.append(clearSymKey.left(16));      //second half key
    m_GcmBaseIV.append(clearSymKey.right(16));  //second half IV

    QByteArray encryptedRespIntegrity, responderIntegrity;
    m_LinkStream >> encryptedRespIntegrity;
    responderIntegrity = gcmDecrypt(encryptedRespIntegrity);
    if(responderIntegrity != m_ResponderIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    updateIntegrityHash(&m_StarterIntegrityHash, clearSymKey+responderIntegrity);
    responderSendStarterIntegrity();
}

void Handshaker::responderSendStarterIntegrity(){ //R:2 forge
    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    connect(m_Link, &AbstractLink::readyRead,
            this, &Handshaker::responderParseHandshakeFinished);

    m_LinkStream << gcmEncrypt(m_StarterIntegrityHash);
    m_Timeout.start();
}

void Handshaker::starterParseStarterIntegrity(){ //S:3 parse
    m_Timeout.stop();
    QByteArray encStarterIntegrity;
    m_LinkStream >> encStarterIntegrity;
    if(isError(encStarterIntegrity)) return;

    QByteArray starterIntegrity = gcmDecrypt(encStarterIntegrity);
    if(starterIntegrity != m_StarterIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    starterSendHandshakeFinished();
}

void Handshaker::starterSendHandshakeFinished(){ //S:3 forge
    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    m_LinkStream << gcmEncrypt(QByteArray(1,(char)HandshakeFinished));
    m_Link->flush(); //send everything packet now
    m_IpFilter->removeBan(m_Link->getHost());
    emit handshakeFinished(true);
}

void Handshaker::responderParseHandshakeFinished(){ //R:3 parse
    m_Timeout.stop();
    disconnect(m_Link, &AbstractLink::readyRead, this, 0);
    QByteArray cypherResponse;
    m_LinkStream >> cypherResponse;
    if(isError(cypherResponse))return;

    QByteArray clearResponse = gcmDecrypt(cypherResponse);

    if(clearResponse.isEmpty()) return;

    else if(clearResponse.size() == 1 && clearResponse.at(0) == (char)HandshakeFinished){
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
    quint16 banTime = getRecievedBanTime(); //implicite cast
    QByteArray errorPacket;
    QDataStream errorPacketStream(&errorPacket, QIODevice::WriteOnly);

    disconnect(m_Link, SIGNAL(readyRead()), this, 0);
    if(banTime > 0){
        if(banTime <= 32767)
            banTime *= 2;
    }else
        banTime = 1;
    errorPacketStream << (quint8)UndefinedError;
    errorPacketStream << banTime;
    m_LinkStream << errorPacket;
    m_IpFilter->addBan(m_Link->getHost(), banTime);
    emit error(err);
    emit handshakeFinished(false);
}

QByteArray Handshaker::generateRandomBlock(uint size){
    QByteArray randomBlock;
    randomBlock.resize(size);
    AutoSeededRandomPool rnd;
    rnd.GenerateBlock((byte*)randomBlock.data(), size);
    return randomBlock;
}

QByteArray Handshaker::gcmDecrypt(QByteArray& cipherText){
    std::string clearText;
    GCM<AES>::Decryption dec;
    QByteArray iv = m_GcmBaseIV;
    iv[iv.size()-1] = iv.at(iv.size()-1) + m_IvOffset + 1;

    dec.SetKeyWithIV((byte*)m_GcmKey.data(), m_GcmKey.size(),
                     (byte*)iv.data(), iv.size());
    try{
        ArraySource((byte*)cipherText.data(), cipherText.size(), true,
                    new AuthenticatedDecryptionFilter(dec,
                                                      new StringSink(clearText)
                                                      )//AuthenticatedDecryptionFilter
                    );  //ArraySource
        m_IvOffset++;
    }catch(CryptoPP::Exception& e){
        qDebug()<< e.what();
        emit error(DataCorrupted);
    }
    return QByteArray(clearText.data(), (int)clearText.size());
}

QByteArray Handshaker::gcmEncrypt(QByteArray& clearText){
    std::string cipherText;
    GCM<AES>::Encryption enc;
    QByteArray iv = m_GcmBaseIV;
    iv[iv.size()-1] = iv.at(iv.size()-1) + m_IvOffset + 1;

    enc.SetKeyWithIV((byte*)m_GcmKey.data(), m_GcmKey.size(),
                     (byte*)iv.data(), iv.size());
    try{
        ArraySource((byte*)clearText.data(), clearText.size(), true,
                    new AuthenticatedEncryptionFilter(enc,
                                                      new StringSink(cipherText)
                                                      ) //AuthenticatedEncryptionFilter
                    ); //ArraySource
        m_IvOffset++;
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
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
        packetStream >> m_RecievedBanTime;
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
            ArraySource((byte*)chunk->data(), chunk->size(), true,
                        new PK_DecryptorFilter(m_RandomGenerator, m_RsaDecryptor,
                                               new StringSink(clearChunk)));

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
            ArraySource((byte*)chunk->data(), chunk->size(), true,
                        new PK_EncryptorFilter(m_RandomGenerator, m_RsaEncryptor,
                                                  new StringSink(cipherChunk)
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
    StringSource(dataToHash, true,
                 new HashFilter(SHA256(),
                                    new ArraySink((byte*)currentHash->data(), currentHash->size())
                                )
                 );
}
