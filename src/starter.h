#ifndef STARTER_H
#define STARTER_H

#include <QString>
#include <QSettings>
#include <QDebug>
#include <QtNetwork>

#include "messenger.h"


class Starter
{
public:
    Starter();
    void opennewConnection(QString);
    ~Starter();


private :
    QSettings *settings;
    QString *name, *IP, *key;
    qint16 *port;
    Messenger *messenger;
    QTcpSocket *socket;

};

#endif // STARTER_H
