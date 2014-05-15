#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <QList>



#include "contact.h"
#include "contactdb.h"
#include "rootlink.h"
#include "packetagent.h"

#include "abstractapp.h"



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
    void onStatusChanged(Contact::Status status);
    void onContactEvent(Contact::Event event);

signals :
    void error(QString error);
    void statusChanged(int id, Contact::Status status);
    void startAppRequest(int appid, int contactId);


private :
    Contact *m_contact;
    QTcpSocket *m_Socket;
    RootLink *m_Root;
    PacketAgent *agent;
    ContactDB *m_ContactDB;


};

#endif // NETWORKMANAGER_H
