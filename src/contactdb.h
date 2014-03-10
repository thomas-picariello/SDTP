#ifndef CONTACTDB_H
#define CONTACTDB_H

#include <QDebug>
#include <QtSql/QtSql>
#include <QList>
#include <QByteArray>
#include "contact.h"

class ContactDB: public QObject
{
    Q_OBJECT
public:
    explicit ContactDB(QObject *parent = 0);
    ~ContactDB();
    bool erase(int id);
    Contact* findById(int id);
    Contact* findByKey(QByteArray key);
    QList<Contact*> getAllContacts();
    int write(Contact *contact);

private :
    QSqlDatabase mDb;

    Q_DISABLE_COPY(ContactDB)
};

#endif // CONTACTDB_H
