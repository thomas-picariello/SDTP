#ifndef CONTACTDB_H
#define CONTACTDB_H

#include <QDebug>
#include <QtSql/QtSql>
#include <QList>
#include <QByteArray>
#include <cryptopp/sha.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include "contact.h"

class ContactDB: public QObject
{
    Q_OBJECT
public:
    explicit ContactDB(QPair<QByteArray, QByteArray> *fileKey, QObject *parent = 0);
    ~ContactDB();
    void updateFileKey();
    bool erase(int id);
    Contact* const findById(int id);
    Contact* const findByKey(QByteArray key);
    QList<Contact*> getAllContacts() const;
    uint add(Contact *contact);
    void commitToDatabase();
    void importDatabase();

signals:
    void error(QString err);

private:
    QMap<uint,Contact*> mContactList;
    QSqlDatabase mDatabase;
    QPair<QByteArray, QByteArray> *mFileKey;

    QByteArray generateHash();
    void initDbTables();
    uint getNextAvailableId();
    QByteArray serializeStringList(QStringList list);
    QStringList deserializeStringList(QByteArray byteArray);
    void openDB();

    Q_DISABLE_COPY(ContactDB)
};

#endif // CONTACTDB_H
