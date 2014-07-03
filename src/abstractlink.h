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
        m_State(Offline),
        m_RecievedPacketCount(0),
        m_SentPacketCount(0)
    {}

    inline State state(){ return m_State; }
    inline quint8 getRecievedPacketCount() const{ return m_RecievedPacketCount; }
    inline quint8 getSentPacketCount() const{ return m_SentPacketCount; }

    virtual bool isSequential(){ return true; }
    virtual QString getHost() const = 0;
    virtual quint16 getPort() const = 0;
    virtual void setHost(const QString &host, const quint16 port) = 0;

signals:
    void stateChanged(AbstractLink::State);
    void error(QString);


public slots:
    void incrementRecievedPacketCount(){ m_RecievedPacketCount++; }
    virtual void onConnected(){ setOpenMode(ReadWrite); emit stateChanged(Online); }
    virtual void onConnectionError(QAbstractSocket::SocketError) = 0;

protected :
    State m_State;
    quint8 m_RecievedPacketCount;
    quint8 m_SentPacketCount;

    virtual qint64 readData(char * data, qint64 maxSize) = 0;
    virtual qint64 writeData(const char * data, qint64 size) = 0;

private:


    Q_DISABLE_COPY(AbstractLink)
};

#endif // ABSTRACTLINK_H
