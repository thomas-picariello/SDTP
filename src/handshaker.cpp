#include "handshaker.h"

Handshaker::Handshaker(QTcpSocket* socket, RsaKeyring* keyring, QObject *parent):
    QObject(parent),
    m_socket(socket),
    m_socketStream(m_socket),
    m_contactDB(NULL),
    m_contact(NULL),
    m_ipFilter(NULL),
    m_rsaKeyring(keyring),
    m_starterIntegrityHash(32, 0),  //256 bits Hash
    m_responderIntegrityHash(32, 0),  //256 bits Hash
    m_mode(UndefinedMode),
    m_ivOffset(0),
    m_receivedBanTime(0)
{
    m_timeout.setSingleShot(true);
    m_timeout.setInterval(5000);    //default Timeout 5s
    resetHandshake();

    connect(&m_timeout, &QTimer::timeout,
            this, &Handshaker::onTimeout);
}

void Handshaker::waitForHandshake(ContactDB *contactDB){
    m_mode = ResponderMode;
    m_contactDB = contactDB;
    if(m_rsaKeyring->hasPrivateKey()){ //security check
        resetHandshake();
    }else{
        emit error(BadPrivateKey);
        connect(m_rsaKeyring, SIGNAL(privateKeyValidated()),
                this, SLOT(resetHandshake()));
    }
}

void Handshaker::startHandshake(Contact *contact){
    m_mode = StarterMode;
    m_contact = contact;
    if(m_rsaKeyring->hasPrivateKey()){ //security check
        resetHandshake();
    }else{
        emit error(BadPrivateKey);
        connect(m_rsaKeyring, SIGNAL(privateKeyValidated()),
                this, SLOT(resetHandshake()));
    }
}

quint16 Handshaker::getRecievedBanTime() const{
    return m_receivedBanTime;
}

QByteArray Handshaker::getGcmBaseIV() const{
    return m_gcmBaseIV;
}

Contact* Handshaker::getContact() const{
    return m_contact;
}

QString Handshaker::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
}

QByteArray Handshaker::getGcmKey() const{
    return m_gcmKey;
}

QString Handshaker::getHost() const{
    return m_socket->peerAddress().toString();
}

Handshaker::Mode Handshaker::getMode() const{
    return m_mode;
}

QTcpSocket *Handshaker::getSocket(){
    return m_socket;
}

void Handshaker::setIpFilter(IpFilter *ipFilter){
    m_ipFilter = ipFilter;
}

void Handshaker::setTimeout(int timeout){
    m_timeout.setInterval(timeout);
}

void Handshaker::resetHandshake(){
    disconnect(m_rsaKeyring, SIGNAL(privateKeyValidated()), this, 0);
    disconnect(m_socket, SIGNAL(readyRead()), this, 0);
    m_starterIntegrityHash.fill(32, 0);
    m_responderIntegrityHash.fill(32, 0);
    m_receivedBanTime = 0;

    switch(m_mode){
    case UndefinedMode:
        try{
            m_rsaDecryptor.AccessKey().Load(ArraySource((byte*)m_rsaKeyring->getStoredPrivateKey().data(),
                                                        m_rsaKeyring->getStoredPrivateKey().size(),
                                                        true));
        }catch(CryptoPP::BERDecodeErr&){
            emit error(BadPrivateKey);
            connect(m_rsaKeyring, SIGNAL(privateKeyValidated()),
                    this, SLOT(resetHandshake()));
        }
        break;
    case StarterMode:
        try{
            m_rsaEncryptor.AccessKey().Load(ArraySource((byte*)m_contact->getKey().data(),
                                                        m_contact->getKey().size(),
                                                        true));
            starterSayHello();
        }catch(CryptoPP::BERDecodeErr&){
            emit error(BadContactKey);
            connect(m_rsaKeyring, SIGNAL(privateKeyValidated()),
                    this, SLOT(resetHandshake()));
        }
        break;
    case ResponderMode:
        connect(m_socket, SIGNAL(readyRead()),
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

    clearTextStream << m_rsaKeyring->generatePublicKey();

    clearTextStream << SUPPORTED_PROTOCOL_VERSION;

    updateIntegrityHash(&m_starterIntegrityHash, (char)secLevel+clearText);

    packetStream << rsaEncrypt(clearText);

    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &Handshaker::starterParseResponderHello);

    m_socketStream << packet;
    m_timeout.start();
}

void Handshaker::responderParseStarterHello(){ //R:1 parse
    QByteArray packet;
    m_socketStream >> packet;
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
    m_contact = m_contactDB->findByKey(key);
    if(m_contact == NULL){
        processError(IdentityCheckFailed);
        return;
    }
    try{
        m_rsaEncryptor.AccessKey().Load(ArraySource((byte*)m_contact->getKey().data(),
                                                    m_contact->getKey().size(),
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

    updateIntegrityHash(&m_starterIntegrityHash, (char)secLevel+clearText);
    responderRespondHello();
}

void Handshaker::responderRespondHello(){ //R:1 forge
    QByteArray clearText;

    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    QByteArray firstHalfSymKey = generateRandomBlock(32);
    m_gcmKey.append(firstHalfSymKey.left(16));  //first half key
    m_gcmBaseIV.append(firstHalfSymKey.right(16));//first half IV
    clearText.append(firstHalfSymKey);

    updateIntegrityHash(&m_responderIntegrityHash, clearText);

    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &Handshaker::responderParseHalfKeyAndResponderIntegrity);

    m_socketStream << rsaEncrypt(clearText);
    m_timeout.start();
}

void Handshaker::starterParseResponderHello(){ //S:2 parse
    m_timeout.stop();
    QByteArray rsaCypherText, clearText;
    m_socketStream >> rsaCypherText;
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

    m_gcmKey.append(clearText.mid(1, 16));  //first half key
    m_gcmBaseIV.append(clearText.mid(1 + 16));  //first half IV
    if(m_gcmKey.size()!=16 || m_gcmBaseIV.size()!=16){
        processError(BadSymmetricKey);
        return;
    }

    updateIntegrityHash(&m_responderIntegrityHash, clearText);
    starterSendHalfKeyAndResponderIntegrity();
}

void Handshaker::starterSendHalfKeyAndResponderIntegrity(){ //S:2 forge
    QByteArray secondHalfSymKey = generateRandomBlock(32);
    m_gcmKey.append(secondHalfSymKey.left(16));     //second half key
    m_gcmBaseIV.append(secondHalfSymKey.right(16)); //second half IV

    updateIntegrityHash(&m_starterIntegrityHash, secondHalfSymKey+m_responderIntegrityHash);

    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &Handshaker::starterParseStarterIntegrity);

    m_socketStream << rsaEncrypt(secondHalfSymKey);
    m_socketStream << gcmEncrypt(m_responderIntegrityHash);

    m_timeout.start();
}

void Handshaker::responderParseHalfKeyAndResponderIntegrity(){ //R:2 parse
    m_timeout.stop();
    QByteArray encryptedSymKey;
    m_socketStream >> encryptedSymKey;
    if(isError(encryptedSymKey)) return;

    QByteArray clearSymKey = rsaDecrypt(encryptedSymKey);
    if(clearSymKey.size()<32){
        processError(BadSymmetricKey);
        return;
    }
    m_gcmKey.append(clearSymKey.left(16));      //second half key
    m_gcmBaseIV.append(clearSymKey.right(16));  //second half IV

    QByteArray encryptedRespIntegrity, responderIntegrity;
    m_socketStream >> encryptedRespIntegrity;
    responderIntegrity = gcmDecrypt(encryptedRespIntegrity);
    if(responderIntegrity != m_responderIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    updateIntegrityHash(&m_starterIntegrityHash, clearSymKey+responderIntegrity);
    responderSendStarterIntegrity();
}

void Handshaker::responderSendStarterIntegrity(){ //R:2 forge
    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &Handshaker::responderParseHandshakeFinished);

    m_socketStream << gcmEncrypt(m_starterIntegrityHash);
    m_timeout.start();
}

void Handshaker::starterParseStarterIntegrity(){ //S:3 parse
    m_timeout.stop();
    QByteArray encStarterIntegrity;
    m_socketStream >> encStarterIntegrity;
    if(isError(encStarterIntegrity)) return;

    QByteArray starterIntegrity = gcmDecrypt(encStarterIntegrity);
    if(starterIntegrity != m_starterIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    starterSendHandshakeFinished();
}

void Handshaker::starterSendHandshakeFinished(){ //S:3 forge
    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    m_socketStream << gcmEncrypt(QByteArray(1,(char)HandshakeFinished));
    emit success();
}

void Handshaker::responderParseHandshakeFinished(){ //R:3 parse
    m_timeout.stop();
    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    QByteArray cypherResponse;
    m_socketStream >> cypherResponse;
    if(isError(cypherResponse))return;

    QByteArray clearResponse = gcmDecrypt(cypherResponse);

    if(clearResponse.isEmpty()) return;

    else if(clearResponse.size() == 1 && clearResponse.at(0) == (char)HandshakeFinished){
        emit success();
    }else{
        processError(UndefinedError);
    }
}

void Handshaker::onTimeout(){
    processError(Timeout);
}

void Handshaker::processError(Error err){
    quint16 banTime = m_receivedBanTime;
    QByteArray errorPacket;
    QDataStream errorPacketStream(&errorPacket, QIODevice::WriteOnly);

    disconnect(m_socket, &QTcpSocket::readyRead, this, 0);
    if(banTime > 0){
        if(banTime <= 32767)
            banTime *= 2;
    }else
        banTime = 1;
    errorPacketStream << (quint8)UndefinedError;
    errorPacketStream << banTime;
    m_socketStream << errorPacket;
    emit error(err);
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
    QByteArray iv = m_gcmBaseIV;
    iv[iv.size()-1] = iv.at(iv.size()-1) + m_ivOffset + 1;

    dec.SetKeyWithIV((byte*)m_gcmKey.data(), m_gcmKey.size(),
                     (byte*)iv.data(), iv.size());
    try{
        ArraySource((byte*)cipherText.data(), cipherText.size(), true,
                    new AuthenticatedDecryptionFilter(dec,
                                                      new StringSink(clearText)
                                                      )//AuthenticatedDecryptionFilter
                    );  //ArraySource
        m_ivOffset++;
    }catch(CryptoPP::Exception&){
        emit error(DataCorrupted);
    }
    return QByteArray(clearText.data(), (int)clearText.size());
}

QByteArray Handshaker::gcmEncrypt(QByteArray& clearText){
    std::string cipherText;
    GCM<AES>::Encryption enc;
    QByteArray iv = m_gcmBaseIV;
    iv[iv.size()-1] = iv.at(iv.size()-1) + m_ivOffset + 1;

    enc.SetKeyWithIV((byte*)m_gcmKey.data(), m_gcmKey.size(),
                     (byte*)iv.data(), iv.size());
    try{
        ArraySource((byte*)clearText.data(), clearText.size(), true,
                    new AuthenticatedEncryptionFilter(enc,
                                                      new StringSink(cipherText)
                                                      ) //AuthenticatedEncryptionFilter
                    ); //ArraySource
        m_ivOffset++;
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
        disconnect(m_socket, SIGNAL(readyRead()), this, 0);
        packetStream >> m_receivedBanTime;
        emit error(UndefinedError);
        return true;
    }
    return false;
}

QByteArray Handshaker::rsaDecrypt(QByteArray& cipherText){
    QByteArray clearText;
    QList<QByteArray*> cipherChunks = splitData(cipherText, (int)m_rsaDecryptor.FixedCiphertextLength());
    foreach(QByteArray *chunk, cipherChunks){
        std::string clearChunk;
        try{
            ArraySource((byte*)chunk->data(), chunk->size(), true,
                        new PK_DecryptorFilter(m_randomGenerator, m_rsaDecryptor,
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
    QList<QByteArray*> clearChunks = splitData(clearText, (int)m_rsaEncryptor.FixedMaxPlaintextLength());
    foreach(QByteArray *chunk, clearChunks){
        std::string cipherChunk;
        try{
            ArraySource((byte*)chunk->data(), chunk->size(), true,
                        new PK_EncryptorFilter(m_randomGenerator, m_rsaEncryptor,
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
