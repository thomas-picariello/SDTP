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


using namespace CryptoPP;

class Handshake : public QObject
{

    Q_OBJECT

public:

    Handshake(QTcpSocket *socket, QObject *parent=0);
    Handshake(QTcpSocket *socket, Contact *contact,  QObject *parent=0);
    ~Handshake();

signals :
    void connectionClosed();
    void handshakeSuccessfull();

public slots :
    void startCheckKey();
    void startCheckCompatibility();
    void respondCheckKey();
    void respondCheckCompatbility();

private :
    QTcpSocket *m_Socket;
    Contact *m_contact;
    QSettings *m_Settings;
    QByteArray *m_StarterMsg;
    QByteArray *m_ResponderMsg;

};

#endif // HANDSHAKE_H
