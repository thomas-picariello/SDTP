#ifndef CONTACTDB_H
#define CONTACTDB_H

#include <QDebug>
#include <QtSql/QtSql>
#include <QList>
#include <QByteArray>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
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

signals:
    void error(QString err);

private :
    QSqlDatabase mMemoryDb;
    QSqlDatabase mDiskDb;
    QPair<QByteArray, QByteArray> *mFileKey;

    QByteArray hashMemoryDb();
    void commitToDiskDb();
    void importDiskDb();
    void initTables();
    QByteArray serializeStringList(QStringList list);
    QStringList deserializeStringList(QByteArray byteArray);

    Q_DISABLE_COPY(ContactDB)
};

#endif // CONTACTDB_H
