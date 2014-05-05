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
#include "packetagent.h"
#include "contactlistwindow.h"




using namespace CryptoPP;

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb,  QObject *parent=0);
    NetworkManager(Contact *contact, ContactDB *starter_contactdb,  QObject *parent=0);
    ~NetworkManager();

    PacketAgent* getRootAgent();
    PacketAgent* newAgent();
    int getContactId();

public slots :
    void chat(QByteArray data);
    void onStatusChanged(ContactListWindow::Status status);
    void onContactEvent(Contact::Event event);

signals :
    void error(QString error);
    void statusChanged(int id, ContactListWindow::Status status);


private :
    Contact *m_contact;
    QTcpSocket *m_Socket;
    AbstractLink *m_Root;
    PacketAgent *agent;
    ContactDB *m_ContactDB;

    MessengerWindow *msgwin;
};

#endif // NETWORKMANAGER_H
