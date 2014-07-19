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
#include "udplink.h"
#include "contact.h"
#include "appuid.h"
#include "appmanager.h"
#include "gcmdevice.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum Error{
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
    void registerApp(AppUID localUID, AbstractApp *app);
    void requestPartnerApp(AppUID localUID);
    void registerAppConnection(AppUID localUID, AppUID distantUID);
    void setContact(Contact *contact){ m_contact = contact; }
    void unregisterApp(AppUID uid);

public slots:
    void sendData(LinkType linkType, QByteArray &data);

signals:
    void disconnected(Contact*);
    void destroyed(NetworkManager*);
    void error(NetworkManager::Error);
    void startAppFor(Contact*, AppUID);

private slots:
    void onStartAppForRequest(AppUID distantUID);
    void routeIncommingData();
    void onTcpDisconnect();

private :
    Contact *m_contact;
    AppManager m_appManager;
    QMap<LinkType, GcmDevice*> m_gcmDevicesList;
    QByteArray m_gcmKey;
    QByteArray m_gcmBaseIv;

    void cleanLinks();
    GcmDevice* getGcmDevice(LinkType linkType);

    Q_DISABLE_COPY(NetworkManager)
};

QDataStream& operator<<(QDataStream &out, const NetworkManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, NetworkManager::Packet &packet);

#endif // NETWORKMANAGER_H
