#ifndef RESPONDER_H
#define RESPONDER_H

#include <QTcpSocket>
#include <QObject>




class Responder
{
    Q_OBJECT
public:
    Responder(QTcpSocket *socket);
    ~Responder();

public slots :
    void startCommunication();
    void readIncomingData();

private :
    QTcpSocket *m_responderSocket;


};

#endif // RESPONDER_H
