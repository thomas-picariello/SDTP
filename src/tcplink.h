#ifndef TCPLINK_H
#define TCPLINK_H

#include "abstractlink.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

class TcpLink : public AbstractLink
{
    Q_OBJECT

public:
    static const LinkType LINKTYPE = TCP;

    explicit TcpLink(QIODevice *parent=0);
    explicit TcpLink(const QString &host, quint16 port, QIODevice *parent=0);
    explicit TcpLink(QTcpSocket *socket, QIODevice *parent=0);
    ~TcpLink();

    virtual qint64 bytesAvailable() const;
    virtual QString getHost() const;
    virtual quint16 getPort() const;
    virtual void setHost(const QString &host, const quint16 port);


    void setSocket(QTcpSocket *socket);

private slots:
    virtual void onConnectionError(QAbstractSocket::SocketError);
    void internalStateChanged(QAbstractSocket::SocketState state);

signals:
    void connected();
    void disconnected();

private:
    QTcpSocket *m_socket;

    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 size);

    void connectSocketSignals();
};

#endif // TCPLINK_H
