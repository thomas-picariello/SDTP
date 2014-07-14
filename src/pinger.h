#ifndef PINGER_H
#define PINGER_H

#include <QDebug>
#include <QObject>
#include <QTimer>

#include "contact.h"
#include "tcplink.h"

class Pinger : public QObject
{
    Q_OBJECT
public:
    explicit Pinger(Contact* contact, QObject *parent = 0);

    Contact* getContact();
    QTcpSocket* getSocket();
    bool hasHost(QString &host) const;
    void setSocket(QTcpSocket* socket);

signals:
    void connected(QTcpSocket* socket);

public slots:
    void start(int delay = 0);
    void stop();

private slots:
    void onConnected();
    void connectToNextHost();

private:
    uint m_hostIndex;
    QTimer m_connectTimer;
    Contact *m_contact;
    QTcpSocket *m_socket;

    Q_DISABLE_COPY(Pinger)
};

#endif // PINGER_H
