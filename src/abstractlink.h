#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include "contact.h"

class AbstractLink
{
public:
    enum STATE{OFFLINE,ONLINE,ERROR};

    AbstractLink(Contact contact);
    AbstractLink(QTcpSocket socket);

    const AbstractLink::STATE state();

public slots:
    void write(QByteArray data);
    QByteArray read();

signals:
    void newdata();
    void connected();
    void disconnected();

private:
    void connect();
    void Handshake();

    Contact mContact;
    QByteArray mData;
    QTcpSocket mSocket;
    AbstractLink::STATE mState;

};

#endif // LINK_H
