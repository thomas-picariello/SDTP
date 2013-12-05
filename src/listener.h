#ifndef LISTENER_H
#define LISTENER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QMapIterator>
#include <QList>
#include "responder.h"

class Listener : public QTcpServer
{
    Q_OBJECT
public:
    explicit Listener(quint16 listenPort, QObject *parent = 0);

private:
    void incomingConnection(qintptr socketDescriptor);

    QList<Responder> *m_responderList;

};

#endif // LISTENER_H
