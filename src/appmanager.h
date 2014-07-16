#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QMap>

#include "typesenums.h"
#include "abstractapp.h"
#include "appuid.h"

class AppManager : public QObject
{
    Q_OBJECT
public:
    Q_ENUMS(AppType)

    enum Command: quint8{
        StartAppCommand = 0x00,
        AppStartedSignal = 0x01,
        AppClosedSignal = 0x02,

        InvalidCommand = 0xFF
    };
    Q_ENUMS(Command)

    enum Error{
        PacketCorrupted,
        UnknownCommand
    };
    Q_ENUMS(Error)

    struct Packet{
        Packet(){}
        Packet(Command cmd, AppUID local, AppUID distant):
            command(cmd), localUID(local), distantUID(distant){}
        Command command;
        AppUID localUID;
        AppUID distantUID;
    };

    explicit AppManager(QObject* parent = 0): QObject(parent){}

    AbstractApp* getApp(AppUID localUID) const;
    AppUID getLocalAppUID(AbstractApp* app) const;
    AppUID getLocalAppUID(AppUID distantAppUID) const;
    AppUID getDistantAppUID(AppUID localAppUID) const;
    AppUID getDistantAppUID(AbstractApp* app) const;
    bool isAppRegistered(AbstractApp* app) const;
    bool isAppConnected(AppUID appUID) const;
    bool registerConnection(AppUID localAppUID, AppUID distantAppUID);
    void sendAppStartedSignal(AppUID localUID);

public slots:
    void readIncommingData(QByteArray &data);
    void requestPartnerApp(AppUID localAppUID);
    void registerApp(AppUID localAppUID, AbstractApp *app);
    void unregisterApp(AppUID localAppUID);

signals:
    void appRoutingReady(AbstractApp* app);
    void distantAppUnregistered(AppUID distantUID);
    void error(Error);
    void sendData(LinkType linkType, QByteArray& data);
    void startAppFor(AppUID distantAppUID);

private:
    QMap<AppUID, AppUID> m_AppConnectionsTable;
    QMap<AppUID, AbstractApp*> m_LocalAppsRegister;

    void sendPacket(Command cmd, AppUID localUID, AppUID distantUID);

    Q_DISABLE_COPY(AppManager)
};

QDataStream& operator<<(QDataStream &out, const AppManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, AppManager::Packet &packet);

#endif // APPMANAGER_H
