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
    static Contact* findById(int id);
    static QList<Contact*> findByName(QString name);
    static QList<Contact*> findByHost(QString host);
    static Contact* findByKey(QByteArray key);
    static QList<Contact*> getContactList();
};

#endif // CONTACTFACTORY_H
