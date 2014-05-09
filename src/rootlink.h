#ifndef ROOTLINK_H
#define ROOTLINK_H

#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "contact.h"
#include "packetagent.h"
#include "contactlistwindow.h"

class RootLink: public QObject
{
    Q_OBJECT
public:
    enum State{OFFLINE,ONLINE,Error};

    RootLink(Contact *contact);
    RootLink(QTcpSocket *socket);


    ~RootLink();

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
    void connected(Contact::Status);
    void disconnected();
    void error(QString);

private:

    QTimer *m_timer;
    PacketAgent *mAgent;
    QByteArray *mData;
    QTcpSocket *mSocket;
    State mState;
    Contact *m_Contact;
    int HostListLength;

};

#endif // LINK_H
