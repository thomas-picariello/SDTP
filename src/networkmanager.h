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
#include "appuid.h"
#include "appmanager.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum Error{
        PacketCorrupted,
        BadTimestamp,
        BadPacketNumber,
        BadSymmetricKey,
        AppNotStarted,
        NoPayload,
        UnregisteredApp
    };
    Q_ENUMS(Error)

    struct Packet{
        quint64 timestamp;
        quint8 packetNumber;
        AppUID destAppUID;
        QByteArray payload;
    };

    NetworkManager(Contact *contact,
                   ContactDB *contactDB,
                   RsaKeyring *keyring,
                   IpFilter *ipFilter,
                   QObject *parent=0);     //Starter
    NetworkManager(QTcpSocket *socket,
                   ContactDB *contactDB,
                   RsaKeyring *keyring,
                   IpFilter *ipFilter,
                   QObject *parent=0);   //Responder
    ~NetworkManager();

    Contact* getContact() const;
    int getContactId() const;
    QString getErrorString(Error err) const;
    //inline Contact::Status getStatus() const{ return m_Contact->getStatus(); } //TODO: implement Contact status
    inline AppManager* getAppManager(){ return &m_AppManager; }
    void setContact(Contact *contact);

public slots :
    void onContactEvent(Contact::Event event);
    void sendData(LinkType linkType, QByteArray &data);

signals :
    void contactStatusChanged(int id, Contact::Status status);
    void destroyed(NetworkManager* networkManager);
    void error(NetworkManager::Error err);
    void newContactId(int id);

private slots:
    void waitForHandshake();
    void doStarterHandshake();
    void onHandshakeFinished(bool successfull);
    void processIncommingData();
    void onDisconnected();
    void onHandshakeError(Handshaker::Error);

private :
    quint64 m_LastTimeStamp;
    quint8 m_LastPacketNumber;
    Contact::Status m_Status;
    Contact *m_Contact;
    ContactDB *m_ContactDB;
    Handshaker *m_Handshaker;
    AppManager m_AppManager;
    Pinger m_Pinger;
    CryptoPP::GCM<CryptoPP::AES>::Decryption *m_GcmDecryptor;
    CryptoPP::GCM<CryptoPP::AES>::Encryption *m_GcmEncryptor;
    QMap<LinkType, AbstractLink*> m_LinkList;
    QMap<AppUID, AbstractApp*> m_AppList;

    void cleanLinks();
    AbstractLink *getLink(LinkType linkType);
    QByteArray gcmDecrypt(QByteArray& cipherText);
    QByteArray gcmEncrypt(QByteArray& clearText);

    Q_DISABLE_COPY(NetworkManager)
};

QDataStream& operator<<(QDataStream &out, const NetworkManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, NetworkManager::Packet &packet);

#endif // NETWORKMANAGER_H
