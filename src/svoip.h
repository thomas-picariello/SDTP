#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include <QByteArray>
#include <QSettings>
#include <QTcpServer>
#include <QMap>
#include <QPair>

#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>

#include "contactdb.h"
#include "contactlistwindow.h"
#include "passwordwindow.h"
#include "networkmanager.h"
#include "ipfilter.h"
#include "pinger.h"
#include "handshakeresponder.h"
#include "handshakestarter.h"
#include "appuid.h"
#include "upnpnat.h"
#include "typesenums.h"
#include "confwizard.h"
#include "messengerapp.h"
#include "voiceapp.h"
#include "videoapp.h"


class SVoIP: public QObject
{
    Q_OBJECT
public: 
    enum IpState{
        NotConnected,
        Handshaking,
        Connected
    };

    enum Error{
        InvalidAppID
    };

	SVoIP(QObject *parent = 0);
    ~SVoIP();

private slots:
    void checkParameters(QByteArray key = QByteArray());
    void onContactEvent(int id, ContactDB::Event event);
    void onDisconnect(Contact* contact);
    void onHandshakeSuccess();
    void onHandshakeError(AbstractHandshake::Error error);
    void onStartAppRequest(Contact *contact, AppType appType);
    void onStartAppForRequest(Contact *contact, AppUID distantUID);
    void onNewConnection();
    void registerNAT(quint16 port,bool connexionType);
    void restartListener();
    void startHandshake(QTcpSocket *socket);
    void startProgram();

signals:
    void error(Error err);

private :
    ConfWizard *m_wizard;
    ContactDB *m_contactDB;
    PasswordWindow *m_passwordWindow;
    ContactListWindow *m_contactListWindow;
    RsaKeyring *m_rsaKeyring;
    QPair<QByteArray,QByteArray> m_fileKey;
    IpFilter m_ipFilter;
    QTcpServer m_listener;
    QMap<Contact*, Pinger*> m_pingerList;
    QMap<QString, AbstractHandshake*> m_handshakeList;

    QMap<Contact*,NetworkManager*> m_networkManagerList;
    QMap<AppUID,AbstractApp*> m_appList;
    QMap<AppUID, Contact*> m_appRegisterTable;

    void displayConfWizard();
    IpState getHostState(QString host);
    QString generateSalt();
    QPair<AppUID,AbstractApp*> startApp(Contact *contact, AppType appType);

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
