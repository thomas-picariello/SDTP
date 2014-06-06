#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QDebug>
#include <QObject>
#include <QMap>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include "typesenums.h"
#include "abstractapp.h"
#include "abstractlink.h"
#include "tcplink.h"
#include "contact.h"
#include "contactdb.h"
#include "pinger.h"
#include "handshaker.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    struct Packet{
        struct EncryptedArea{
            quint64 timestamp;
            byte packetNumber;
            AbstractApp::AppUID appUID;
            QByteArray payload;
        };
        byte hash[32]; //256 bits for sha256 hash
    };

    NetworkManager(Contact *contact,
                   ContactDB *contactDB,
                   QPair<QByteArray, QByteArray> *fileKey,
                   QObject *parent=0);     //Starter
    NetworkManager(QTcpSocket *socket,
                   ContactDB *contactDB,
                   QPair<QByteArray, QByteArray> *fileKey,
                   QObject *parent=0);   //Responder
    ~NetworkManager();

    Contact* getContact() const;
    int getContactId() const;
    Contact::Status getStatus() const;
    void registerApp(AbstractApp::AppUID uid, AbstractApp *app);
    void setContact(Contact *contact);
    void unregisterApp(AbstractApp::AppUID uid);

public slots :
    void onContactEvent(Contact::Event event);
    void sendData(QByteArray &data, LinkType linkType);

signals :
    void contactStatusChanged(int id, Contact::Status status);
    void error(QString error);
    void startRootApp(int contactId);

private slots:
    void doResponderHandshake(TcpLink *link);
    void doStarterHandshake(QString host);
    void onHandshakeFinished(bool successfull);
    void processIncommingData();
    void onDisconnected();

private :
    Contact::Status m_Status;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    Handshaker *m_Handshaker;
    Pinger m_Pinger;
    QPair<QByteArray*, QByteArray*> m_AesKey;
    QMap<LinkType, AbstractLink*> m_LinkList;
    QMap<AbstractApp::AppUID, AbstractApp*> m_AppList;
    QMap<AbstractApp::AppUID, LinkType> m_RoutingTable;

    Packet createPacket(QByteArray &payload);
    void cleanLinks();
    AbstractLink *getLink(LinkType linkType);
    void connectSignals();
    Packet parsePacket(QByteArray &packet);
    QByteArray serializePacket(Packet &packet);

    Q_DISABLE_COPY(NetworkManager)
};

#endif // NETWORKMANAGER_H
