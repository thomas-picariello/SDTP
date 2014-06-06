#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include <QIODevice>
#include <QAbstractSocket>

class AbstractLink : public QIODevice
{
    Q_OBJECT

public:
    enum State{
        Offline,
        Online,
        Error
    };

    AbstractLink(QObject *parent=0):
        QIODevice(parent),
        m_State(Offline)
    {}

    inline State state(){ return m_State; }
    virtual bool isSequential(){ return true; }
    virtual QPair<QString,quint16> getHost() const = 0;
    virtual void setHost(QPair<QString,quint16> &host) = 0;

signals:
    void stateChanged(AbstractLink::State);
    void error(QString);


public slots:
    virtual void onConnected(){ setOpenMode(ReadWrite); emit stateChanged(Online); }
    virtual void onConnectionError(QAbstractSocket::SocketError) = 0;

protected :
    State m_State;

    qint64 readData(char * data, qint64 maxSize) = 0;
    qint64 writeData(const char * data, qint64 size) = 0;

    Q_DISABLE_COPY(AbstractLink)
};

#endif // ABSTRACTLINK_H
