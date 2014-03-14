#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include <QSettings>
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "contact.h"
#include "contactfactory.h"
#include <QSettings>
#include <QCoreApplication>
#include "contactdb.h"


using namespace CryptoPP;

class Handshake : public QObject
{

    Q_OBJECT

public:

    Handshake(QTcpSocket *socket, ContactDB *contactdb, QObject *parent=0);
    Handshake(QTcpSocket *socket, ContactDB *contactdb,Contact *contact,  QObject *parent=0);
    Contact* getContact();
    QByteArray getkey();
    ~Handshake();

signals :
    void connectionClosed();
    void handshakeSuccessfull();

public slots :
    void startCheckKey();
    void startCheckCompatibility();
    void respondCheckKey();
    void respondCheckCompatbility();
    void finishHandshake();


private :
    ContactDB *m_contactdb;
    QTcpSocket *m_Socket;
    Contact *m_contact;
    QSettings *m_Settings;
    QByteArray *m_key;
    QStringList *m_CompatibleVersions;

};

#endif // HANDSHAKE_H
