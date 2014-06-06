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
    explicit Pinger(QObject *parent = 0);
    explicit Pinger(Contact *contact, TcpLink *link, QObject *parent = 0);

    QString getActiveHost() const;
    void setLink(AbstractLink *link);
    void setContact(Contact *contact);
    bool start();

signals:
    void connected(QString host);

public slots:


private slots:
    void connectToNextHost();

private:
    uint m_HostIndex;
    QString m_ActiveHost;
    QTimer m_ConnectTimer;
    Contact *m_Contact;
    TcpLink *m_Link;

    Q_DISABLE_COPY(Pinger)
};

#endif // PINGER_H
