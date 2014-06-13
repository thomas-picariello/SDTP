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
#include "messengerapp.h"

class SVoIP: public QObject
{
    Q_OBJECT
public: 
	SVoIP(QObject *parent = 0);
    ~SVoIP();

public slots:

    void onIncommingConnection();
    void restartListener();
    void deleteFromList(QObject *object);
    void updateNetworkManagerId(int newId);
    void updateContactStatus(int id, Contact::Status status);
    void onContactEvent(int id, Contact::Event event);
    void startApp(int contactId, AppType appType);
    inline void startRootApp(int contactId){ startApp(contactId, Root); }

private slots:
    void checkParameters(QByteArray key = QByteArray());
    void startProgram();

signals:
    void error(QString err);

private :
    ContactDB *mContactDB;
    PasswordWindow *mPasswordWindow;
    ContactListWindow *mContactListWindow;
    RsaKeyring *mRsaKeyring;
    QPair<QByteArray,QByteArray> mFileKey;
    QTcpServer mListener;
    QMap<int,NetworkManager*> mNetworkManagerList;
    QMap<QPair<int,AbstractApp::AppUID>, AbstractApp*> mAppList;

    void displayFirstStartWizard();
    void connectNetworkManagerSignals(NetworkManager *networkManager);
    QString generateSalt();

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
