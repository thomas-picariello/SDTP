#ifndef PACKETASSEMBLER_H
#define PACKETASSEMBLER_H

#include <QObject>
#include "contact.h"
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <QDateTime>
#include <QDataStream>
#include <QtEndian>
#include "handshake.h"

class PacketAssembler : public QObject
{
    Q_OBJECT
public:

    explicit PacketAssembler(Contact* ,QObject *parent = 0);
    ~PacketAssembler();

    QByteArray assemble(QByteArray , quint8);
    QByteArray disassemble(QByteArray);

    quint8 getID();

signals:
    QByteArray assembled();
    QByteArray disassembled();

private :
    void refreshKey();
    void sync();
    void encrypt();
    void decrypt();


    Contact *m_contact;
    QByteArray m_data;
    quint8 m_appID;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption mCfbAesEnc;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption mCfbAesDec;
    quint8 m_PacketCounter;
    quint16 m_TimeStamp;
    QTime *m_Time;
    QDateTime *m_dateTime;
    QByteArray key;
    Handshake *m_handshake;
};

#endif // PACKETASSEMBLER_H
