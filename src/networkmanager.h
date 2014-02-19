#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include <QSettings>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <QDateTime>
#include <QDataStream>
#include <QtEndian>

#include "voip.h"
#include "message.h"
#include "messengerwindow.h"
#include "contact.h"
#include "handshake.h"
#include "qjrtp.h"



using namespace CryptoPP;

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum AppID{MESSENGER,VOIP ,SYSTEM,SYNC,UNKNOWN}; // 0-10 are reserved IDs.

    NetworkManager(QTcpSocket *socket,  QObject *parent=0);
    NetworkManager(Contact *contact,  QObject *parent=0);
    ~NetworkManager();

public slots :
    void readIncomingData();
    void sendData(QByteArray,quint8);
    void error(QAbstractSocket::SocketError);
    void voipCall();

    void onVoIPReadyRead();
    void onConnect();//for starter
    void onIdentified();

private :
    QTcpSocket *m_Socket;
    QSettings *m_settings;
    QJrtp *m_QJrtp;
    VoIP *m_voip;
    Handshake *m_handshake;
    Contact *m_contact;
    MessengerWindow *m_MessengerWindow;
    QByteArray mAesKey;
    QByteArray mAesIv;
    quint8 m_PacketCounter;
    quint16 m_TimeStamp;
    QTime *m_Time;
    QDateTime *m_dateTime;
    CFB_Mode<AES>::Encryption mCfbAesEnc;
    CFB_Mode<AES>::Decryption mCfbAesDec;
};

#endif // NETWORKMANAGER_H
