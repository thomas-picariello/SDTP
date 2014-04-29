#include "contactdb.h"

ContactDB::ContactDB(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent), mFileKey(fileKey)
{
    mMemoryDb = QSqlDatabase::addDatabase("QSQLITE", "memoryDb");
    mMemoryDb.setDatabaseName(":memory:");
    if(!mMemoryDb.open())
        emit error(mMemoryDb.lastError().text());

    mDiskDb = QSqlDatabase::addDatabase("QSQLITE", "diskDb");
    mDiskDb.setDatabaseName("contacts.db3");
    if(!mDiskDb.open())
        emit error(mDiskDb.lastError().text());

    initTables();
    importDiskDb();
}

ContactDB::~ContactDB(){
    mMemoryDb.close();
    mDiskDb.close();
}

bool ContactDB::erase(int id){
    QSqlQuery eraseMemoryEntryQuery(mMemoryDb);
    QSqlQuery eraseDiskEntryQuery(mDiskDb);
    bool result;

    eraseMemoryEntryQuery.prepare("DELETE FROM contacts WHERE id=:id");
    eraseMemoryEntryQuery.bindValue(":id", id);
    result = eraseMemoryEntryQuery.exec();
    if(!result)
        emit error(eraseMemoryEntryQuery.lastError().text());

    //TODO: once commit delete everything on disk, remove this.
    eraseDiskEntryQuery.prepare("DELETE FROM contacts WHERE id=:id");
    eraseDiskEntryQuery.bindValue(":id", id);
    result |= eraseDiskEntryQuery.exec();
    if(!result)
        emit error(eraseDiskEntryQuery.lastError().text());
    //(to here)
    return result;
}

Contact* ContactDB::findById(int id){
    QSqlQuery query(mMemoryDb);
    query.prepare("SELECT name,hosts,port,key "
                  "FROM contacts WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    if(query.exec()){
        query.next();
        QString name = query.value(0).toString();
        QStringList hostList = deserializeStringList(query.value(1).toByteArray());
        quint16 port = query.value(2).toUInt();
        QByteArray key = query.value(3).toByteArray();
        return new Contact(id, name, hostList, port, key, this);
    }else{
        emit error(query.lastError().text());
        return NULL;
    }
}

Contact* ContactDB::findByKey(QByteArray key){
    QSqlQuery query(mMemoryDb);
    query.prepare("SELECT id,name,hosts,port "
                  "FROM contacts WHERE key=:key");
    query.bindValue(":key", key);
    if(query.exec()){
        query.next();
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QStringList hostList = deserializeStringList(query.value(2).toByteArray());
        quint16 port = query.value(3).toUInt();
        return new Contact(id, name, hostList, port, key, this);
    }else{
        emit error(query.lastError().text());
        return NULL;
    }
}

QList<Contact*> ContactDB::getAllContacts(){
    QList<Contact*> returnList;
    QSqlQuery query(mMemoryDb);
    if(query.exec("SELECT id,name,hosts,port,key FROM contacts;")){
        while(query.next()){
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            QStringList hostList = deserializeStringList(query.value(2).toByteArray());
            quint16 port = query.value(3).toUInt();
            QByteArray key = query.value(4).toByteArray();
            returnList.append(new Contact(id, name, hostList, port, key, this));
        }
    }else
        emit error(query.lastError().text());
    return returnList;
}

int ContactDB::write(Contact *contact){
    contact->setParent(this); //avoid contact to be deleted automatically
    QSqlQuery query(mMemoryDb);
    if(contact->getId() == 0){
        query.prepare("INSERT INTO contacts (name,hosts,port,key) "
                      "VALUES (:name,:hosts,:port,:key)");
    }else{
        query.prepare("UPDATE contacts "
                      "SET name=:name,hosts=:hosts,port=:port,key=:key "
                      "WHERE id=:id");
        query.bindValue(":id", contact->getId());
    }
    query.bindValue(":name", contact->getName());
    query.bindValue(":hosts", serializeStringList(contact->getHostsList()));
    query.bindValue(":port", contact->getPort());
    query.bindValue(":key", contact->getKey());
    if(!query.exec())
          emit error(query.lastError().text());

    commitToDiskDb();
    delete(contact);
    return 0;
}

QByteArray ContactDB::hashMemoryDb(){
    QSqlQuery query(mMemoryDb);
    QString dbContent;
    query.exec("SELECT id,name,hosts,port,key FROM contacts;");
    while(query.next()){
        dbContent.append(query.value(0).toString()); //id
        dbContent.append(query.value(1).toString()); //name
        dbContent.append(query.value(2).toString()); //serialized host list
        dbContent.append(query.value(3).toString()); //port
        dbContent.append(query.value(4).toString()); //key
    }
    QString salt = QSettings("settings.ini", QSettings::IniFormat).value("encryption/salt").toString();
    dbContent.append(salt);
    std::string digest;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource(dbContent.toStdString(),
                           true,
                           new CryptoPP::HashFilter(hash,
                               new CryptoPP::StringSink(digest)
                           ));
    return QString::fromStdString(digest).toUtf8();
}

void ContactDB::initTables(){
    QSqlQuery memoryDbInitQuery(mMemoryDb);
    QSqlQuery diskDbInitQuery(mDiskDb);
    QString createContactTableQuery = "CREATE TABLE IF NOT EXISTS contacts("
                                           "id      INTEGER PRIMARY KEY, "
                                           "name    TEXT, "
                                           "hosts   BLOB, "
                                           "port    INTEGER, "
                                           "key     BLOB "
                                       ");";
    memoryDbInitQuery.exec(createContactTableQuery);
    diskDbInitQuery.exec(createContactTableQuery);

    QString createIntegrityTableQuery = "CREATE TABLE IF NOT EXISTS integrity("
                                            "hash   BLOB"
                                        ");";
    diskDbInitQuery.exec(createIntegrityTableQuery);
}

void ContactDB::importDiskDb(){
    QSqlQuery readDiskQuery(mDiskDb);
    QSqlQuery writeMemoryQuery(mMemoryDb);
    readDiskQuery.exec("SELECT id,name,hosts,port,key FROM contacts;");
    while(readDiskQuery.next()){
        QString id = readDiskQuery.value(0).toString();             //id
        QString name = readDiskQuery.value(1).toString();           //name
        QByteArray hostList = readDiskQuery.value(2).toByteArray(); //serialized host list
        QString port = readDiskQuery.value(3).toString();           //port
        QByteArray key = readDiskQuery.value(4).toByteArray();      //key

        //TODO: decrypt values here (not id)

        writeMemoryQuery.prepare("INSERT INTO contacts (id,name,hosts,port,key) "
                                 "VALUES (:id,:name,:hosts,:port,:key);");
        writeMemoryQuery.bindValue(":id", id);
        writeMemoryQuery.bindValue(":name", name);
        writeMemoryQuery.bindValue(":hosts", hostList);
        writeMemoryQuery.bindValue(":port", port);
        writeMemoryQuery.bindValue(":key", key);
        if(!writeMemoryQuery.exec())
            emit error(writeMemoryQuery.lastError().text());
    }
    QByteArray storedHash;
    if(!readDiskQuery.exec("SELECT hash FROM integrity;"))
        emit error(readDiskQuery.lastError().text());
    else{
        readDiskQuery.next();
        storedHash = readDiskQuery.value(0).toByteArray();
        if(storedHash != hashMemoryDb()){
            emit error(tr("Error: Contact database content corrupted"));
            writeMemoryQuery.exec("DELETE FROM contacts"); //flush memory database
        }
    }
}

void ContactDB::commitToDiskDb(){
    QSqlQuery readMemoryQuery(mMemoryDb);
    QSqlQuery writeDiskQuery(mDiskDb);

    //TODO: clear contacts table on disk

    if(!readMemoryQuery.exec("SELECT id,name,hosts,port,key FROM contacts;"))
        emit error(readMemoryQuery.lastError().text());
    while(readMemoryQuery.next()){
        QString id = readMemoryQuery.value(0).toString();             //id
        QString name = readMemoryQuery.value(1).toString();           //name
        QByteArray hostList = readMemoryQuery.value(2).toByteArray(); //serialized host list
        QString port = readMemoryQuery.value(3).toString();           //port
        QByteArray key = readMemoryQuery.value(4).toByteArray();      //key

        //TODO: encrypt data (not id)

        writeDiskQuery.prepare("REPLACE INTO contacts "
                      "VALUES (:id,:name,:hosts,:port,:key);");
        writeDiskQuery.bindValue(":id", id);
        writeDiskQuery.bindValue(":name", name);
        writeDiskQuery.bindValue(":hosts", hostList);
        writeDiskQuery.bindValue(":port", port);
        writeDiskQuery.bindValue(":key", key);
        if(!writeDiskQuery.exec())
            emit error(writeDiskQuery.lastError().text());
    }

    QSqlQuery updateHashQuery(mDiskDb);
    if(updateHashQuery.exec("DELETE FROM integrity;")) //clear the table
        emit error(updateHashQuery.lastError().text());
    updateHashQuery.prepare("INSERT INTO integrity(hash) "
                            "VALUES (:hash);");
    updateHashQuery.bindValue(":hash", hashMemoryDb());
    if(!updateHashQuery.exec())
        emit error(updateHashQuery.lastError().text());
}

QByteArray ContactDB::serializeStringList(QStringList stringList){
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << stringList;
    return byteArray;
}

QStringList ContactDB::deserializeStringList(QByteArray byteArray){
    QStringList result;
    QDataStream in(&byteArray, QIODevice::ReadOnly);
    in >> result;
    return result;
}
