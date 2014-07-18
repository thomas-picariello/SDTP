#include "handshakeresponder.h"

HandshakeResponder::HandshakeResponder(QTcpSocket* socket, ContactDB* contactDB, IpFilter* ipFilter, RsaKeyring* keyring, QObject* parent):
    AbstractHandshake(socket, ipFilter, keyring, parent),
    m_contactDB(contactDB)
{
    connect(m_socket, &QTcpSocket::readyRead,
            this, &HandshakeResponder::responderParseStarterHello);
}

void HandshakeResponder::responderParseStarterHello(){ //R:1 parse
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

void HandshakeResponder::responderRespondHello(){ //R:1 forge
    QByteArray clearText;

    clearText.append(SUPPORTED_PROTOCOL_VERSION);

    QByteArray firstHalfSymKey = generateRandomBlock(32);
    m_gcmKey.append(firstHalfSymKey.left(16));  //first half key
    m_gcmBaseIV.append(firstHalfSymKey.right(16));//first half IV
    clearText.append(firstHalfSymKey);

    updateIntegrityHash(&m_responderIntegrityHash, clearText);

    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &HandshakeResponder::responderParseHalfKeyAndResponderIntegrity);

    m_socketStream << rsaEncrypt(clearText);
    m_timeout.start();
}

void HandshakeResponder::responderParseHalfKeyAndResponderIntegrity(){ //R:2 parse
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
    //TODO remove debug
    qDebug() << "Responder: encrypted responder integrity" << encryptedRespIntegrity.toBase64();
    qDebug() << "Responder: clear responder integrity" << responderIntegrity.toBase64();

    if(responderIntegrity != m_responderIntegrityHash){
        processError(DataCorrupted);
        return;
    }

    updateIntegrityHash(&m_starterIntegrityHash, clearSymKey+responderIntegrity);
    responderSendStarterIntegrity();
}

void HandshakeResponder::responderSendStarterIntegrity(){ //R:2 forge
    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &HandshakeResponder::responderParseHandshakeFinished);

    m_socketStream << gcmEncrypt(m_starterIntegrityHash);
    m_timeout.start();
}

void HandshakeResponder::responderParseHandshakeFinished(){ //R:3 parse
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
