#ifndef LISTENER_H
#define LISTENER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QMapIterator>
#include "responder.h"

class Listener : public QTcpServer
{
    Q_OBJECT
public:
    explicit Listener(quint16 listenPort, QObject *parent = 0);
    ~Listener();

signals:
    
public slots:
    void connectionClosed(quintptr socketDescriptor);
    void closeConnection(QString name);

private:
    QMap<quintptr,Responder*> mRespondersList;
    void incomingConnection(qintptr socketDescriptor);

};

#endif // LISTENER_H
