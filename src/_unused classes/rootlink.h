#ifndef ROOTLINK_H
#define ROOTLINK_H

#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "contact.h"
#include "packetagent.h"
#include "contactlistwindow.h"
#include "abstractlink.h"

class RootLink: public AbstractLink
{
    Q_OBJECT
public:

    RootLink(Contact *contact, QObject *parent=0);
    RootLink(QTcpSocket *socket, QObject *parent=0);

    ~RootLink();
    virtual QByteArray readAll();

public slots:
    void send(QByteArray &data);
    void handshake();
    void onConnected();
    void tryConnect();
    void onConnectionError(QAbstractSocket::SocketError error);
    //PacketAgent* getagent();

private:
    QTimer *m_Timer;
    //PacketAgent *mAgent;
    QByteArray *mData;
    //QTcpSocket *mSocket;
    Contact *m_Contact;
    uint m_HostListIndex;
};

#endif // LINK_H
