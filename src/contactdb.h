#ifndef CONTACTDB_H
#define CONTACTDB_H

#include <QDebug>
#include <QtSql/QtSql>
#include <QList>
#include <QByteArray>
#include <cryptopp/authenc.h>
#include "contact.h"

class ContactDB: public QObject
{
    Q_OBJECT
public:
    explicit ContactDB(QPair<QByteArray, QByteArray> *fileKey, QObject *parent = 0);
    ~ContactDB();
    bool erase(int id);
    Contact* findById(int id);
    Contact* findByKey(QByteArray key);
    QList<Contact*> getAllContacts();
    int write(Contact *contact);

private :
    QSqlDatabase mDb;
    QPair<QByteArray, QByteArray> *mFileKey;

    Q_DISABLE_COPY(ContactDB)
};

#endif // CONTACTDB_H
