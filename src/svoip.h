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
#include "rootapp.h"
#include "confwizard.h"
#include "messengerapp.h"
#include "ipfilter.h"

class SVoIP: public QObject
{
    Q_OBJECT
public: 
    enum Error{
        InvalidAppID
    };

	SVoIP(QObject *parent = 0);
    ~SVoIP();

public slots:
    void restartListener();
    void onNetworkManagerDestroy(NetworkManager *networkManager);
    void updateNetworkManagerId(int newId);
    void updateContactStatus(int id, Contact::Status status);
    void onContactEvent(int id, Contact::Event event);
    void startApp(QList<Contact *> contactList, AppType appType);
    void startRootApp(Contact *contact);

private slots:
    void checkParameters(QByteArray key = QByteArray());
    void startProgram();
    void onIpAccepted(QTcpSocket* socket);
    void onNewConnection();

signals:
    void error(Error err);

private :
    ConfWizard *m_wizard;
    ContactDB *mContactDB;
    PasswordWindow *mPasswordWindow;
    ContactListWindow *mContactListWindow;
    RsaKeyring *mRsaKeyring;
    QPair<QByteArray,QByteArray> mFileKey;
    IpFilter mIpFilter;
    QTcpServer mListener;
    QMap<int,NetworkManager*> mNetworkManagerList;
    QMap<AbstractApp::AppUID,AbstractApp*> mAppList; //TODO switch to QHash for unique key

    void displayFirstStartWizard();
    void connectNetworkManagerSignals(NetworkManager *networkManager);
    QString generateSalt();

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
