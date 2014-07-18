#include "handshakestarter.h"

HandshakeStarter::HandshakeStarter(QTcpSocket* socket, Contact* contact, IpFilter* ipFilter, RsaKeyring* keyring, QObject* parent) :
    AbstractHandshake(socket, ipFilter, keyring, parent)
{
    m_contact = contact;
    try{
        m_rsaEncryptor.AccessKey().Load(ArraySource((byte*)m_contact->getKey().data(),
                                                    m_contact->getKey().size(),
                                                    true));
        starterSayHello();
    }catch(CryptoPP::BERDecodeErr&){
        emit error(BadContactKey);
    }
}

void HandshakeStarter::starterSayHello(){ //S:1 forge
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
            this, &HandshakeStarter::starterParseResponderHello);

    m_socketStream << packet;
    m_timeout.start();
}

void HandshakeStarter::starterParseResponderHello(){ //S:2 parse
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

void HandshakeStarter::starterSendHalfKeyAndResponderIntegrity(){ //S:2 forge
    QByteArray secondHalfSymKey = generateRandomBlock(32);
    m_gcmKey.append(secondHalfSymKey.left(16));     //second half key
    m_gcmBaseIV.append(secondHalfSymKey.right(16)); //second half IV

    updateIntegrityHash(&m_starterIntegrityHash, secondHalfSymKey+m_responderIntegrityHash);

    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    connect(m_socket, &AbstractLink::readyRead,
            this, &HandshakeStarter::starterParseStarterIntegrity);

    m_socketStream << rsaEncrypt(secondHalfSymKey);
    QByteArray encryptedResponderIntegrity = gcmEncrypt(m_responderIntegrityHash);
    m_socketStream << encryptedResponderIntegrity;
    //TODO: remove debug
    qDebug() << "Starter: encrypted responder integrity" << encryptedResponderIntegrity.toBase64();
    qDebug() << "Starter: clear responder integrity" << m_responderIntegrityHash.toBase64();

    m_timeout.start();
}

void HandshakeStarter::starterParseStarterIntegrity(){ //S:3 parse
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

void HandshakeStarter::starterSendHandshakeFinished(){ //S:3 forge
    disconnect(m_socket, &AbstractLink::readyRead, this, 0);
    m_socketStream << gcmEncrypt(QByteArray(1,(char)HandshakeFinished));
    emit success();
}
