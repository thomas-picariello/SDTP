#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QDebug>
#include <QObject>
#include <QMap>

#include <cryptopp/aes.h>
#include <cryptopp/authenc.h>
#include <cryptopp/gcm.h>

#include "typesenums.h"
#include "abstractapp.h"
#include "abstractlink.h"
#include "tcplink.h"
#include "contact.h"
#include "contactdb.h"
#include "pinger.h"
#include "handshaker.h"
#include "ipfilter.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum Error{

    };
    Q_ENUMS(Error)

    struct Packet{
        quint64 timestamp;
        byte packetNumber;
        AbstractApp::AppUID appUID;
        QByteArray payload;
    };

    NetworkManager(Contact *contact,
                   ContactDB *contactDB,
                   RsaKeyring *keyring,
                   QObject *parent=0);     //Starter
    NetworkManager(QTcpSocket *socket,
                   ContactDB *contactDB,
                   RsaKeyring *keyring,
                   IpFilter* ipFilter,
                   QObject *parent=0);   //Responder
    ~NetworkManager();

    Contact* getContact() const;
    int getContactId() const;
    QString getErrorString(Error err) const;
    Contact::Status getStatus() const;
    void registerApp(AbstractApp::AppUID uid, AbstractApp *app);
    void setContact(Contact *contact);
    void unregisterApp(AbstractApp::AppUID uid);

public slots :
    void onContactEvent(Contact::Event event);
    void sendData(QByteArray &data, LinkType linkType);

signals :
    void contactStatusChanged(int id, Contact::Status status);
    void destroyed(NetworkManager* networkManager);
    void error(NetworkManager::Error err);
    void newContactId(int id);
    void startRootApp(int contactId);

private slots:
    void waitForHandshake();
    void doStarterHandshake();
    void onHandshakeFinished(bool successfull);
    void processIncommingData();
    void onDisconnected();
    void onHandshakeError(Handshaker::Error);

private :
    Contact::Status m_Status;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    Handshaker *m_Handshaker;
    Pinger m_Pinger;
    CryptoPP::GCM<CryptoPP::AES>::Decryption *m_GcmDecryptor;
    CryptoPP::GCM<CryptoPP::AES>::Encryption *m_GcmEncryptor;
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
