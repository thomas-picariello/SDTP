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


using namespace CryptoPP;

class Handshake : public QObject
{

    Q_OBJECT

public:

    Handshake(QTcpSocket *socket, QObject *parent=0);
    Handshake(QTcpSocket *socket, Contact contact,  QObject *parent=0);
    ~Handshake();

signals :
    void connectionClosed();
    void handshakeSuccessfull();

private :
    QTcpSocket *m_Socket;
    Contact contact;
};

#endif // HANDSHAKE_H
