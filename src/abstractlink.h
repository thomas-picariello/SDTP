#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

//#include <QObject>
#include <QAbstractSocket>

class AbstractLink : public QObject
{
    Q_OBJECT

public:
    enum State{
        Offline,
        Online,
        Error
    };

    AbstractLink():
        m_State(Offline),
        m_Socket(NULL)
    {}

    AbstractLink(QAbstractSocket *socket):
        m_State(Offline),
        m_Socket(socket)
    {}

    State state(){ return m_State; }
    virtual QByteArray readAll() = 0;

signals:
    void stateChanged();
    void newDataAvailable();
    void error(QString);


public slots:
    virtual void write(QByteArray data) = 0;
    virtual void onConnected() = 0;
    virtual void onConnectionError(QAbstractSocket::SocketError) = 0;

protected :
    QAbstractSocket *m_Socket;
    State m_State;

};

#endif // ABSTRACTLINK_H
