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
#include "typesenums.h"
#include "confwizard.h"
#include "messengerapp.h"
#include "ipfilter.h"
#include "pinger.h"
#include "handshaker.h"
#include "appuid.h"
#include "upnpnat.h"

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

public slots:
    void onContactEvent(int id, Contact::Event event);
    void onHandshakeSuccess();
    void onHandshakeError(Handshaker::Error error);
    void onNetworkManagerDestroy(NetworkManager *networkManager);
    void registerNAT(quint16 port,bool connexionType);
    void restartListener();
    AbstractApp* startApp(Contact *contact, AppType appType);
    AbstractApp* startAppFor(Contact *contact, AppUID distantUID);
    void startHandshaker(QTcpSocket *socket);
    void updateContactStatus(int id, Contact::Status status);

private slots:
    void checkParameters(QByteArray key = QByteArray());
    void onNewConnection();
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
    QMap<QString, Handshaker*> m_handshakerList;
    QMap<int,NetworkManager*> m_networkManagerList;
    QMap<AppUID,AbstractApp*> m_appList;

    void displayConfWizard();
    IpState getHostState(QString host);
    QString generateSalt();

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
