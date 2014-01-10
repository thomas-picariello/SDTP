#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H




#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include "voip.h"
#include "message.h"
#include "messengerwindow.h"
#include "contact.h"

class NetworkManager : public QObject
{

    Q_OBJECT

public:
    NetworkManager(QTcpSocket *socket,  QObject *parent=0);
    ~NetworkManager();


public slots :
    void readIncomingData();
    void sendData(QByteArray);
    void error(QAbstractSocket::SocketError);
    void call();


private :
    QTcpSocket *m_Socket;
    VoIP *voip;
    //Contact *contact;
    MessengerWindow *m_MessengerWindow;

};

#endif // NETWORKMANAGER_H
