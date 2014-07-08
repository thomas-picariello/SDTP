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

    State state(){ return m_State; }

    virtual qint64 bytesAvailable() const = 0;
    virtual bool flush(){ return false; }   //return false if not reimplemented
    virtual bool isSequential(){ return true; }
    virtual QString getHost() const = 0;
    virtual quint16 getPort() const = 0;
    virtual void setHost(const QString &host, const quint16 port) = 0;

signals:
    void stateChanged(AbstractLink::State);
    void openModeChanged(OpenMode);
    void error(QString);

private slots:
    virtual void onConnected(){}
    virtual void onConnectionError(QAbstractSocket::SocketError) = 0;

    void privateOnConnected(){
        setOpenMode(ReadWrite);
        emit openModeChanged(ReadWrite);
        emit stateChanged(Online);
        onConnected();
    }

protected :
    State m_State;

    virtual qint64 readData(char * data, qint64 maxSize) = 0;
    virtual qint64 writeData(const char * data, qint64 size) = 0;

private:


    Q_DISABLE_COPY(AbstractLink)
};

#endif // ABSTRACTLINK_H
