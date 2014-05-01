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
#include "qjrtp.h"
#include "contactdb.h"
#include "abstractlink.h"



using namespace CryptoPP;

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum AppID{MESSENGER,VOIP ,SYSTEM,SYNC,UNKNOWN}; // 0-10 are reserved IDs.

    NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb,  QObject *parent=0);
    NetworkManager(Contact *contact, ContactDB *starter_contactdb,  QObject *parent=0);
    ~NetworkManager();

signals :
    void error(QString error);


private :
    ContactDB *m_Contact;
    QTcpSocket *m_Socket;
    AbstractLink *m_Root;
};

#endif // NETWORKMANAGER_H
