#ifndef PACKETAGENT_H
#define PACKETAGENT_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QPair>
#include <QMap>

#include "abstractapp.h"
#include "typesenums.h"
#include "abstractlink.h"
#include "tcplink.h"

class PacketAgent: public QObject
{
    Q_OBJECT

public:
    PacketAgent();
    PacketAgent(QPair<QByteArray,QByteArray> key);// key = key + IV
    ~PacketAgent();

    uint logApp(AbstractApp* app, AppType appTypeId);

public slots :
    void incomingData(QByteArray data);//link

signals:
    void senddata(QByteArray data);//link


private:

    void routeToApp(AbstractApp::AppUID appUID, QByteArray data);
    void routeToLink(AbstractApp::AppUID appUID, QByteArray data);

    void encrypt();
    void decrypt();
    void addheader();

    void addHash();
    void makeHash();

    void getHash();
    void parseHeader();
    bool verifyHash();

    QByteArray mContent;
    QByteArray mData;
    QByteArray mHash;
    QByteArray mKey;
    QByteArray mIV;

    QMap<AbstractApp::AppUID, AbstractApp*> mAppMap;
    QMap<AbstractApp::AppUID, AbstractApp::AppUID> mRouteMap;

    Q_DISABLE_COPY(PacketAgent)
};

#endif // PACKET_H
