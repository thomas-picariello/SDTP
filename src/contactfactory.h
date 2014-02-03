#ifndef CONTACTFACTORY_H
#define CONTACTFACTORY_H

#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QHostAddress>
#include "contact.h"

class ContactFactory
{
public:
    static Contact* findById(int id, QObject *parent = 0);
    static QList<Contact*> findByName(QString name, QObject *parent = 0);
    static QList<Contact*> findByHost(QString host, QObject *parent = 0);
    static Contact* findByKey(QByteArray key, QObject *parent = 0);
    static QList<Contact*> getContactList(QObject *parent = 0);
};

#endif // CONTACTFACTORY_H
