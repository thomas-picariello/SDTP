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
                   QTcpSocket *socket,
                   QByteArray gcmKey,
                   QByteArray gcmBaseIV,
                   QObject *parent = 0);
    ~NetworkManager();

    QString getHost();
    Contact* getContact() const;
    int getContactId() const;
    QString getErrorString(Error err) const;
//    State getState() const;
    //inline Contact::Status getStatus() const{ return m_Contact->getStatus(); } //TODO: implement Contact status
    void registerApp(AppUID localUID, AbstractApp *app);
    void registerAppConnection(AppUID localUID, AppUID distantUID);
    void setContact(Contact *contact){ m_Contact = contact; }
    void unregisterApp(AppUID uid);

public slots :
    void onContactEvent(Contact::Event event);
    void sendData(LinkType linkType, QByteArray &data);

signals :
    void contactStatusChanged(int id, Contact::Status status);
    void destroyed(NetworkManager* networkManager);
    void error(NetworkManager::Error err);
//    void newContactId(int id);
    void startApp(Contact* contact, AppType type);
    void startAppFor(Contact *contact, AppUID distantAppUID);

private slots:
//    void waitForHandshake();
//    void doStarterHandshake();
//    void onHandshakeFinished(bool successfull);
    void onStartAppRequest(AppType type);
    void onStartAppForRequest(AppUID distantUID);
    void onRouteReady(AbstractApp* app);
    void routeIncommingData();
    void onTcpDisconnect();
//    void onHandshakeError(Handshaker::Error);

private :
//    State m_State;
    Contact::Status m_Status;
    Contact *m_Contact;
//    ContactDB *m_ContactDB;
//    Handshaker *m_Handshaker;
    AppManager m_AppManager;
//    Pinger m_Pinger;
    QMap<LinkType, GcmDevice*> m_GcmDevicesList;
    QByteArray m_GcmKey;
    QByteArray m_GcmBaseIv;

    void cleanLinks();
    GcmDevice* getGcmDevice(LinkType linkType);
//    QByteArray gcmDecrypt(QByteArray& cipherText);
//    QByteArray gcmEncrypt(QByteArray& clearText);

    Q_DISABLE_COPY(NetworkManager)
};

QDataStream& operator<<(QDataStream &out, const NetworkManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, NetworkManager::Packet &packet);

#endif // NETWORKMANAGER_H
