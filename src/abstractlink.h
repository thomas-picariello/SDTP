#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include "contact.h"
#include "packetagent.h"

class AbstractLink: public QObject
{
    Q_OBJECT
public:
    enum State{OFFLINE,ONLINE,Error};

    AbstractLink(Contact contact);
    AbstractLink(QTcpSocket *socket);
    ~AbstractLink();

    State state();

public slots:
    void write(QByteArray data);
    QByteArray read();

signals:
    void newdata();
    void connected();
    void disconnected();

private:
    void connect();
    void handshake();

    PacketAgent *mAgent;
    Contact mContact;
    QByteArray *mData;
    QTcpSocket *mSocket;
    State mState;

};

#endif // LINK_H
