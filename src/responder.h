#ifndef RESPONDER_H
#define RESPONDER_H

#include <QDebug>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>

class Responder : public QThread
{
    Q_OBJECT
public:

    explicit Responder(quintptr socketDescriptor, QObject *parent = 0);
    ~Responder();

    quintptr getSocketDescriptor() const;
    void closeConnection();
    
signals:
    void disconnected(quintptr socketDescriptor);
    void error(QAbstractSocket::SocketError);
    
public slots:
    void onReadyRead();
    void onDisconnect();

protected:
    QTcpSocket *mSocket;
    bool mQuitFlag;

    void run();

private:
    const quintptr mSocketDescriptor;
    
};

#endif // RESPONDER_H
