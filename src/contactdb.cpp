#include "contactdb.h"

ContactDB::ContactDB(QObject *parent):
    QObject(parent)
{
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setDatabaseName("contacts.db3");
    mDb.open();
    QSqlQuery query(mDb);
    QString strQuery = "CREATE TABLE IF NOT EXISTS contacts("
                           "id   INTEGER PRIMARY KEY, "
                           "name TEXT, "
                           "host TEXT, "
                           "port INTEGER, "
                           "key  TEXT "
                       ");";
    query.exec(strQuery);
}

ContactDB::~ContactDB(){
    mDb.close();
}

bool ContactDB::erase(int id){
    QSqlQuery query(mDb);
    query.prepare("DELETE FROM contacts WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

Contact* ContactDB::findById(int id){
    QSqlQuery query(mDb);
    query.prepare("SELECT name,host,port,key "
                  "FROM contacts WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    if(query.exec()){
        query.next();
        QString name = query.value(0).toString();
        QString host = query.value(1).toString();
        quint16 port = query.value(2).toUInt();
        QByteArray key = query.value(3).toByteArray();
        return new Contact(id, name, host, port, key, this);
    }else
        return NULL;
}

Contact* ContactDB::findByKey(QByteArray key){
    QSqlQuery query(mDb);
    query.prepare("SELECT id,name,host,port "
                  "FROM contacts WHERE key=:key");
    query.bindValue(":key", key);
    if(query.exec()){
        query.next();
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString host = query.value(2).toString();
        quint16 port = query.value(3).toUInt();
        return new Contact(id, name, host, port, key, this);
    }else
        return NULL;
}

QList<Contact*> ContactDB::getAllContacts(){
    QList<Contact*> returnList;
    QSqlQuery query(mDb);
    query.exec("SELECT id,name,host,port,key FROM contacts;");
    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString host = query.value(2).toString();
        quint16 port = query.value(3).toUInt();
        QByteArray key = query.value(4).toByteArray();
        returnList.append(new Contact(id,name,host,port, key, this));
    }
    return returnList;
}

int ContactDB::write(Contact *contact){
    contact->setParent(this); //avoid contact to be deleted automatically
    QSqlQuery query(mDb);
    if(contact->getId() == 0){
        query.prepare("INSERT INTO contacts (name,host,port,key) "
                      "VALUES (:name,:host,:port,:key)");
        query.bindValue(":name", contact->getName());
        query.bindValue(":host", contact->getHost());
        query.bindValue(":port", contact->getPort());
        query.bindValue(":key", contact->getKey());
        if(!query.exec())
            qDebug()<<query.lastError();
    }else{
        query.prepare("UPDATE contacts "
                      "SET name=:name,host=:host,port=:port,key=:key "
                      "WHERE id=:id");
        query.bindValue(":id", contact->getId());
        query.bindValue(":name", contact->getName());
        query.bindValue(":host", contact->getHost());
        query.bindValue(":port", contact->getPort());
        query.bindValue(":key", contact->getKey());
        if(!query.exec())
              qDebug()<<query.lastError();
    }
    delete(contact);
    return 0;
}

