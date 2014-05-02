#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include "contact.h"
#include "packetagent.h"
#include "contactlistwindow.h"

class AbstractLink: public QObject
{
    Q_OBJECT
public:
    enum State{OFFLINE,ONLINE,Error};

    AbstractLink(Contact *contact);
    AbstractLink(QTcpSocket *socket);
    ~AbstractLink();

    State state();

public slots:
    void write(QByteArray data);
    void read();
    void handshake();
    void onConnected();
    void tryConnect();
    void onSocketError(QAbstractSocket::SocketError error);
    PacketAgent* getagent();

signals:
    void statechanged();
    void newdata();
    void connected(ContactListWindow::Status);
    void disconnected();
    void error(QString);

private:

    PacketAgent *mAgent;
    QByteArray *mData;
    QTcpSocket *mSocket;
    State mState;
    Contact *m_Contact;
    int HostListLength;

};

#endif // LINK_H
