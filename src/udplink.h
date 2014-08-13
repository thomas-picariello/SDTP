/**
 * @brief UdpLink is a Link specifically designed for UDP Sockets
 */

#ifndef UDPLINK_H
#define UDPLINK_H

#include "abstractlink.h"
#include <QUdpSocket>

class UdpLink : public AbstractLink
{
    Q_OBJECT

public:
    static const LinkType LINKTYPE = TCP;

    explicit UdpLink(const QString &host, quint16 hostPort, quint16 listenPort, QIODevice *parent=0);
    ~UdpLink();

    virtual qint64 bytesAvailable() const;
    virtual QString getHost() const;
    virtual quint16 getPort() const;
    virtual void setHost(const QString &host, const quint16 port);

private slots:
    virtual void onConnectionError(QAbstractSocket::SocketError);

signals:

private:
    QUdpSocket *m_inputSocket;
    QUdpSocket *m_outputSocket;

    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 size);

    void connectSocketSignals();
};

#endif // UDPLINK_H
