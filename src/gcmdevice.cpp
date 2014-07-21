#include "gcmdevice.h"

GcmDevice::GcmDevice(AbstractLink *link, QObject *parent) :
    QIODevice(parent),
    m_Link(link),
    m_LinkStream(m_Link),
    m_LastSequenceNumber(0)
{
    m_Link->setParent(this); //take ownership of the link
    connect(m_Link, &AbstractLink::openModeChanged,
            this, &GcmDevice::onLinkOpenModeChanged);
    connect(m_Link, &AbstractLink::readyRead,
            this, &GcmDevice::readFromLink);
    setOpenMode(m_Link->openMode());
}

AbstractLink* GcmDevice::getLink(){
    return m_Link;
}

void GcmDevice::setLink(AbstractLink* link, bool destroyOld){
    if(destroyOld)
        delete m_Link;
    m_Link = link;
    setOpenMode(m_Link->openMode());
}

QByteArray GcmDevice::getKey() const{
    return m_Key;
}

QByteArray GcmDevice::getBaseIV() const{
    return m_BaseIV;
}

void GcmDevice::setKeyAndBaseIV(QByteArray key, QByteArray baseIV){
    m_Key = key;
    m_BaseIV = baseIV;
}

bool GcmDevice::open(OpenMode mode){
    bool linkOk = m_Link->open(mode);
    if(linkOk)
        setOpenMode(mode);
    return linkOk;
}

void GcmDevice::onLinkOpenModeChanged(OpenMode openMode){
    setOpenMode(openMode);
}

void GcmDevice::readFromLink(){
    m_DataBuffer.append(m_Link->readAll());
    const quint32 cypherPayloadSize = qFromBigEndian<quint32>((uchar*)m_DataBuffer.mid(sizeof(quint64), sizeof(quint32)).data());
    const quint32 packetSize = sizeof(quint64) + sizeof(quint32) + cypherPayloadSize;
    while(m_DataBuffer.size() >= (int)packetSize){
        QByteArray packet = m_DataBuffer.left(packetSize);
        QDataStream packetStream(&packet, QIODevice::ReadOnly);
        quint64 seqNum;
        QByteArray cypherPayload;
        packetStream >> seqNum;
        packetStream >> cypherPayload;
        m_PacketList.append(qMakePair(seqNum, cypherPayload));
        m_DataBuffer.remove(0, packetSize);
    }
    emit readyRead();
}

QByteArray GcmDevice::decrypt(quint64 seqNum, QByteArray& gcmPacket){
    QByteArray clearText;
    std::string clearTextStr;

    QByteArray seqNumBytes(sizeof(seqNum), 0x00);
    qToBigEndian<quint64>(seqNum, (uchar*)seqNumBytes.data());
    QByteArray packetSizeBytes(sizeof(quint32), 0x00);
    qToBigEndian<quint32>(gcmPacket.size(), (uchar*)packetSizeBytes.data());
    QByteArray iv = generateIV(seqNum);

    QByteArray mac = gcmPacket.right(GCM_TAG_SIZE);
    QByteArray cypherData = gcmPacket.left(gcmPacket.size()-GCM_TAG_SIZE);

    GCM<AES>::Decryption dec;
    try{
        dec.SetKeyWithIV((byte*)m_Key.data(), m_Key.size(),
                         (byte*)iv.data(), iv.size());
        AuthenticatedDecryptionFilter decFilter(dec,
                                                new StringSink(clearTextStr),
                                                AuthenticatedDecryptionFilter::MAC_AT_BEGIN,
                                                GCM_TAG_SIZE);

        decFilter.ChannelPut(DEFAULT_CHANNEL, (byte*)mac.data(), mac.size());
        decFilter.ChannelPut(AAD_CHANNEL, (byte*)seqNumBytes.data(), seqNumBytes.size());
        //decFilter.ChannelPut(AAD_CHANNEL, (byte*)packetSizeBytes.data(), packetSizeBytes.size());
        decFilter.ChannelPut(DEFAULT_CHANNEL, (byte*)cypherData.data(), cypherData.size());

        decFilter.ChannelMessageEnd(AAD_CHANNEL);
        decFilter.ChannelMessageEnd(DEFAULT_CHANNEL);

        clearText.append(clearTextStr.data(), (int)clearTextStr.size());
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
    }
    return clearText;
}

QByteArray GcmDevice::encrypt(quint64 seqNum, QByteArray& clearText){
    QByteArray cypherText;
    std::string cypherTextStr;

    QByteArray seqNumBytes(sizeof(seqNum), 0x00);
    qToBigEndian<quint64>(seqNum, (uchar*)seqNumBytes.data());
    QByteArray iv = generateIV(seqNum);

    GCM<AES>::Encryption enc;
    try{
        enc.SetKeyWithIV((byte*)m_Key.data(), m_Key.size(),
                         (byte*)iv.data(), iv.size());

        AuthenticatedEncryptionFilter encFilter (enc, new StringSink(cypherTextStr), false, GCM_TAG_SIZE);

        encFilter.ChannelPut(AAD_CHANNEL, (byte*)seqNumBytes.data(), seqNumBytes.size());
        //TODO: find encrypted packet size
        encFilter.ChannelMessageEnd(AAD_CHANNEL);

        encFilter.ChannelPut(DEFAULT_CHANNEL, (byte*)clearText.data(), clearText.size());
        encFilter.ChannelMessageEnd(DEFAULT_CHANNEL);
        cypherText.append(cypherTextStr.data(), (int)cypherTextStr.size());
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
    }
    return cypherText;
}

qint64 GcmDevice::readData(char *data, qint64 maxlen){
    QPair<int,QByteArray> packet;
    if(!m_PacketList.isEmpty()){
        packet = m_PacketList.takeFirst();
        const quint64 seqNum = packet.first;
        if(seqNum > m_LastSequenceNumber){  //TODO: what on overflow ?
            QByteArray clearText = decrypt(seqNum, packet.second);
            if(clearText.size() <= maxlen){
                memcpy(data, clearText.data(), clearText.size());
                m_LastSequenceNumber = seqNum;
                return clearText.size();
            }
        }
    }
    return 0;
}

qint64 GcmDevice::writeData(const char *data, qint64 len){
    QByteArray clearData(data, len);
    quint64 seqNum = m_LastSequenceNumber + 1;
    QByteArray cypherText = encrypt(seqNum, clearData);
    QByteArray packetBytes;
    QDataStream(&packetBytes, QIODevice::WriteOnly) << seqNum << cypherText;
    m_Link->write(packetBytes);
    m_LastSequenceNumber++;
    return sizeof(seqNum) + sizeof(quint32) + cypherText.size();
}

QByteArray GcmDevice::generateIV(quint64 sequenceNumber){
    SHA256 hash;
    QByteArray data;
    QByteArray digest(hash.DigestSize(), 0x00);
    QByteArray seqNum(sizeof(sequenceNumber), 0x00);
    qToBigEndian<quint64>(sequenceNumber, reinterpret_cast<uchar*>(seqNum.data()));

    data.append(m_Key);
    data.append(m_BaseIV);
    data.append(seqNum);

    ArraySource((byte*)data.data(), data.size(), true,
                new HashFilter(hash,
                               new ArraySink((byte*)digest.data(), digest.size())
                               )
                );
    return digest;
}
