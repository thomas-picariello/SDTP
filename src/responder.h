#ifndef RESPONDER_H
#define RESPONDER_H

#include <QTcpSocket>
#include <QObject>
#include "message.h"
#include "messengerwindow.h"

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
    void error(QAbstractSocket::SocketError);

private :
    QTcpSocket *m_responderSocket;
    MessengerWindow *m_messenger_window;
    QByteArray *m_incomingData;
};

#endif // RESPONDER_H
