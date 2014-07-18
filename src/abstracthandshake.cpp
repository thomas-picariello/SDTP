#include "abstracthandshake.h"

AbstractHandshake::AbstractHandshake(QTcpSocket* socket, IpFilter* ipFilter, RsaKeyring* keyring, QObject* parent) :
    QObject(parent),
    m_socket(socket),
    m_socketStream(m_socket),
    m_ipFilter(ipFilter),
    m_rsaKeyring(keyring),
    m_contact(NULL),
    m_starterIntegrityHash(32, 0),  //256 bits Hash
    m_responderIntegrityHash(32, 0),  //256 bits Hash
    m_ivOffset(0)
{
    m_timeout.setSingleShot(true);
    m_timeout.setInterval(5000);    //default Timeout 5s
    try{
        m_rsaDecryptor.AccessKey().Load(ArraySource((byte*)m_rsaKeyring->getStoredPrivateKey().data(),
                                                    m_rsaKeyring->getStoredPrivateKey().size(),
                                                    true));
    }catch(CryptoPP::BERDecodeErr&){
        emit error(BadPrivateKey);
    }
    connect(&m_timeout, &QTimer::timeout,
            this, &AbstractHandshake::onTimeout);
}

quint16 AbstractHandshake::getRecievedBanTime() const{
    return m_receivedBanTime;
}

QByteArray AbstractHandshake::getGcmBaseIV() const{
    return m_gcmBaseIV;
}

Contact* AbstractHandshake::getContact() const{
    return m_contact;
}

QString AbstractHandshake::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
}

QByteArray AbstractHandshake::getGcmKey() const{
    return m_gcmKey;
}

QString AbstractHandshake::getHost() const{
    return m_socket->peerAddress().toString();
}

QTcpSocket* AbstractHandshake::getSocket(){
    return m_socket;
}

void AbstractHandshake::setIpFilter(IpFilter *ipFilter){
    m_ipFilter = ipFilter;
}

void AbstractHandshake::setTimeout(int timeout){
    m_timeout.setInterval(timeout);
}

void AbstractHandshake::onTimeout(){
    processError(Timeout);
}

void AbstractHandshake::processError(Error err){
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

QByteArray AbstractHandshake::generateRandomBlock(uint size){
    QByteArray randomBlock;
    randomBlock.resize(size);
    AutoSeededRandomPool rnd;
    rnd.GenerateBlock((byte*)randomBlock.data(), size);
    return randomBlock;
}

QByteArray AbstractHandshake::gcmDecrypt(QByteArray& cipherText){
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
                    );//ArraySource
        m_ivOffset++;
    }catch(CryptoPP::Exception&){
        emit error(DataCorrupted);
    }
    return QByteArray(clearText.data(), (int)clearText.size());
}

QByteArray AbstractHandshake::gcmEncrypt(QByteArray& clearText){
    std::string cipherText;
    GCM<AES>::Encryption enc;
    QByteArray iv = m_gcmBaseIV;
    iv[iv.size()-1] = iv.at(iv.size()-1) + (m_ivOffset + 1);

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

bool AbstractHandshake::isError(const QByteArray &packet){
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

QByteArray AbstractHandshake::rsaDecrypt(QByteArray& cipherText){
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

QByteArray AbstractHandshake::rsaEncrypt(QByteArray& clearText){
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
            cipherText.clear();
        }
    }
    qDeleteAll(clearChunks);
    clearChunks.clear();
    return cipherText;
}

QList<QByteArray*> AbstractHandshake::splitData(const QByteArray &data, const uint chunkSize){
    QList<QByteArray*> chunks;
    quint64 offset = 0;
    do{
        chunks.append(new QByteArray(data.mid(offset, chunkSize)));
        offset += chunkSize;
    }while(offset < data.size());
    return chunks;
}

void AbstractHandshake::updateIntegrityHash(QByteArray *currentHash, const QByteArray &data){
    std::string dataToHash;
    dataToHash.append(currentHash->constData(), currentHash->size());
    dataToHash.append(data.constData(), data.size());
    StringSource(dataToHash, true,
                 new HashFilter(SHA256(),
                                    new ArraySink((byte*)currentHash->data(), currentHash->size())
                                )
                 );
}
