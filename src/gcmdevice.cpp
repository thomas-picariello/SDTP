#include "gcmdevice.h"

GcmDevice::GcmDevice(AbstractLink *link, QObject *parent) :
    QIODevice(parent),
    m_Link(link),
    m_LastSequenceNumber(0)
{
    m_Link->setParent(this); //take ownership of the link
    setOpenMode(m_Link->openMode());

    connect(m_Link, &AbstractLink::openModeChanged,
            this, &GcmDevice::onLinkOpenModeChanged);
    connect(m_Link, &AbstractLink::readyRead,
            this, &GcmDevice::readyRead);
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

qint64 GcmDevice::readData(char *data, qint64 maxlen){
    std::string clearData;
    QByteArray rawData = m_Link->readAll();
    //check packet size
    if(rawData.size() < 24) //seqNum size (8 bytes) + gcm default tag size (16 bytes)
        return 0;
    QByteArray seqNumBytes = rawData.left(8);
    QByteArray cypherData = rawData.mid(8);
    quint64 seqNum = qFromBigEndian<quint64>((uchar*)seqNumBytes.data());

    if(seqNum > m_LastSequenceNumber){  //TODO: what on overflow ?
        QByteArray iv = generateIV(seqNum);
        GCM<AES>::Decryption dec;
        try{
            dec.SetKeyWithIV((byte*)m_Key.data(), m_Key.size(),
                             (byte*)iv.data(), iv.size());
            AuthenticatedDecryptionFilter decFilter(dec, new StringSink(clearData));
            decFilter.ChannelPut(AAD_CHANNEL, (byte*)seqNumBytes.data(), seqNumBytes.size());
            decFilter.ChannelMessageEnd(AAD_CHANNEL);
            decFilter.ChannelPut(DEFAULT_CHANNEL, (byte*)cypherData.data(), cypherData.size());
            decFilter.ChannelMessageEnd(DEFAULT_CHANNEL);

            if((int)clearData.size() <= maxlen){
                memcpy(data, clearData.data(), clearData.size());
                m_LastSequenceNumber = seqNum;
                return clearData.size();
            }
        }catch(CryptoPP::Exception& e){
            qDebug()<<e.what();
        }
    }
    return 0;
}

qint64 GcmDevice::writeData(const char *data, qint64 len){
    QByteArray clearData(data, len);
    std::string cypherData;
    quint64 seqNum = m_LastSequenceNumber + 1;
    QByteArray seqNumBytes(sizeof(seqNum), 0x00);
    qToBigEndian<quint64>(seqNum, (uchar*)seqNumBytes.data());
    QByteArray iv = generateIV(seqNum);

    GCM<AES>::Encryption enc;
    try{
        enc.SetKeyWithIV((byte*)m_Key.data(), m_Key.size(),
                         (byte*)iv.data(), iv.size());

        AuthenticatedEncryptionFilter encFilter (enc, new StringSink(cypherData));
        encFilter.ChannelPut(AAD_CHANNEL, (byte*)seqNumBytes.data(), seqNum);
        encFilter.ChannelMessageEnd(AAD_CHANNEL);
        encFilter.ChannelPut(DEFAULT_CHANNEL, (byte*)clearData.data(), clearData.size());
        encFilter.ChannelMessageEnd(DEFAULT_CHANNEL);

        if(m_Link->write(cypherData.data(), cypherData.size())){
            m_LastSequenceNumber++;
            return len;
        }
    }catch(CryptoPP::Exception& e){
        qDebug()<<e.what();
    }
    return 0;
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
