#ifndef STARTER_H
#define STARTER_H

#include <QString>
#include <QSettings>
#include <QDebug>
#include "messenger.h"


class Starter
{
public:
    Starter();
    void opennewConnection(QString);
    ~Starter();


private :
    QSettings *settings;
    QString *name, *IP, *port, *key;
    Messenger *messenger;


};

#endif // STARTER_H
