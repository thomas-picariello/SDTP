#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "packetagent.h"
#include "contact.h"

class AbstractLink : public QObject
{
    Q_OBJECT
public:
    enum State{OFFLINE,ONLINE,Error};

    AbstractLink(QString ip, PacketAgent*agent);
    State state();
    ~AbstractLink();

signals:
    void stateChanged();
    void newdata();
    void error(QString);


public slots:
    void write(QByteArray data);
    void read();
    void onConnected();
    void tryConnect();
    void onConnectionError();
    PacketAgent* getagent();

protected :
    QTimer *m_timer;
    PacketAgent *m_Agent;
    QAbstractSocket *m_Socket;
    State *m_State;
    int HostListLength;




};

#endif // ABSTRACTLINK_H
