#ifndef TCPLINK_H
#define TCPLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "abstractlink.h"
#include "contact.h"

class TcpLink : public AbstractLink
{
    Q_OBJECT
public:
    TcpLink(const QString &host, quint16 port);
    TcpLink(QTcpSocket *socket);
    ~TcpLink();

    virtual QByteArray readAll();

public slots:
    virtual void write(QByteArray data);
    virtual void onConnected();
    virtual void onConnectionError(QAbstractSocket::SocketError);

};

#endif // TCPLINK_H
