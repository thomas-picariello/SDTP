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
#include "gcmdevice.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum Error{
        BadTimestamp,
        BadPacketNumber,
        BadSymmetricKey,
        NoPayload,
        PacketCorrupted,
        UnconnectedApp,
        UnregisteredApp
    };
    Q_ENUMS(Error)

    struct Packet{
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

    Contact* getContact() const{ return m_Contact; }
    int getContactId() const;
    QString getErrorString(Error err) const;
    //inline Contact::Status getStatus() const{ return m_Contact->getStatus(); } //TODO: implement Contact status
    void registerApp(AppUID uid, AbstractApp *app);
    void setContact(Contact *contact){ m_Contact = contact; }
    void unregisterApp(AppUID uid);

public slots :
    void onContactEvent(Contact::Event event);
    void sendData(LinkType linkType, QByteArray &data);

signals :
    void contactStatusChanged(int id, Contact::Status status);
    void destroyed(NetworkManager* networkManager);
    void error(NetworkManager::Error err);
    void newContactId(int id);
    void startApp(Contact* contact, AppType type);

private slots:
    void waitForHandshake();
    void doStarterHandshake();
    void onHandshakeFinished(bool successfull);
    void onStartApp(AppType type){ if(m_Contact) emit startApp(m_Contact, type); }
    void routeIncommingData();
    void onTcpDisconnect();
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
    QMap<LinkType, GcmDevice*> m_GcmDevicesList;
    QByteArray m_GcmKey;
    QByteArray m_GcmBaseIv;

    void cleanLinks();
    GcmDevice* getGcmDevice(LinkType linkType);
    QByteArray gcmDecrypt(QByteArray& cipherText);
    QByteArray gcmEncrypt(QByteArray& clearText);

    Q_DISABLE_COPY(NetworkManager)
};

QDataStream& operator<<(QDataStream &out, const NetworkManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, NetworkManager::Packet &packet);

#endif // NETWORKMANAGER_H
