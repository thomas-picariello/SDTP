#ifndef RESPONDER_H
#define RESPONDER_H

#include <QTcpSocket>
#include <QObject>
#include "messenger.h"




class Responder : public QObject
{

    Q_OBJECT

public:
    Responder(QTcpSocket *socket,  QObject *parent=0 );
    ~Responder();

public slots :
    void startCommunication();
    void readIncomingData();
    void sendData(QByteArray);

private :
    QTcpSocket *m_responderSocket;
    Messenger *messenger;
    QByteArray *m_incomingData;


};

#endif // RESPONDER_H
