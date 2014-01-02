#ifndef STARTER_H
#define STARTER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QtNetwork>
#include "contact.h"
#include "messengerwindow.h"

class Starter : public QObject
{
    Q_OBJECT

public:
    explicit Starter(QObject *parent = 0);
    void openConnection(QString name);
    ~Starter();


public slots:
    void onConnect();
    void onDataRecieved();
    void onError(QAbstractSocket::SocketError error);
    void onSendData(QByteArray data);

private :
    MessengerWindow *mMessengerWindow;
    QTcpSocket *mSocket;

};

#endif // STARTER_H
