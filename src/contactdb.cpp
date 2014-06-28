#include "contactdb.h"

ContactDB::ContactDB(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent), mFileKey(fileKey)
{
    initDbTables();
    importDatabase();
}

ContactDB::~ContactDB(){
    qDeleteAll(mContactList);
}

void ContactDB::updateFileKey(){
    commitToDatabase();
}

bool ContactDB::erase(int id){
    delete mContactList.value(id);
    mContactList.remove(id);
    QSqlQuery eraseDiskEntryQuery(mDatabase);
    eraseDiskEntryQuery.prepare("DELETE FROM contacts WHERE id=:id");
    eraseDiskEntryQuery.bindValue(":id", id);
    return eraseDiskEntryQuery.exec();
}

Contact* const ContactDB::findById(int id){
    return mContactList.value(id, NULL);
}

Contact* const ContactDB::findByKey(QByteArray key){
    foreach(Contact* contact, mContactList){
        if(contact->getKey() == key)
            return contact;
    }
    return NULL;
}

QList<Contact*> ContactDB::getAllContacts() const{
    return mContactList.values();
}

uint ContactDB::add(Contact *contact){
    uint id = getNextAvailableId();
    contact->setParent(this);
    contact->setId(id);
    mContactList.insert(id, contact);
    return id;
}

QByteArray ContactDB::generateHash(){
    QByteArray content;
    QDataStream contentStream(&content, QIODevice::WriteOnly);
    foreach(Contact *contact, mContactList){
        contentStream << contact->getId()
                      << contact->getName()
                      << contact->getHostsList()
                      << contact->getPort()
                      << contact->getKey();
    }
    QByteArray salt = QByteArray::fromBase64(QSettings("settings.ini", QSettings::IniFormat).value("encryption/salt").toByteArray());
    contentStream << salt;
    std::string digest;
    CryptoPP::SHA256 hash;
    CryptoPP::ArraySource((byte*)content.data(), content.size(), true,
                           new CryptoPP::HashFilter(hash,
                               new CryptoPP::StringSink(digest)
                           ));
    return QByteArray(digest.c_str(), (int)digest.size());
}

void ContactDB::initDbTables(){
    openDB();
    QSqlQuery diskDbInitQuery(mDatabase);
    QString createContactTableQuery = "CREATE TABLE IF NOT EXISTS contacts("
                                           "id      INTEGER PRIMARY KEY, "
                                           "name    BLOB, "
                                           "hosts   BLOB, "
                                           "port    INTEGER, "
                                           "key     BLOB "
                                       ");";
    diskDbInitQuery.exec(createContactTableQuery);

    QString createIntegrityTableQuery = "CREATE TABLE IF NOT EXISTS integrity("
                                            "hash   BLOB"
                                        ");";
    diskDbInitQuery.exec(createIntegrityTableQuery);
    mDatabase.close();
}

uint ContactDB::getNextAvailableId(){
    uint id = 1; //id start at 1 in DB (-> min. id = 1)
    QList<uint> idList;
    foreach(Contact* contact, mContactList)
        idList << contact->getId();
    while(idList.contains(id))
        id++;
    return id;
}

void ContactDB::importDatabase(){
    openDB();
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption dec;
    if(!mFileKey->first.isEmpty()){
        dec.SetKeyWithIV((byte*)mFileKey->first.data(),
                         mFileKey->first.length(),
                         (byte*)mFileKey->second.data());
    }
    QSqlQuery readDiskQuery(mDatabase);
    readDiskQuery.exec("SELECT id,name,hosts,port,key FROM contacts;");
    while(readDiskQuery.next()){
        uint id = readDiskQuery.value(0).toUInt();                  //id
        QByteArray name = readDiskQuery.value(1).toByteArray();     //name
        QByteArray hostList = readDiskQuery.value(2).toByteArray(); //serialized host list
        QByteArray port = readDiskQuery.value(3).toByteArray();     //port
        QByteArray key = readDiskQuery.value(4).toByteArray();      //key

        if(!mFileKey->first.isEmpty()){
            dec.ProcessString((byte*)name.data(), name.length());
            dec.ProcessString((byte*)hostList.data(), hostList.length());
            dec.ProcessString((byte*)port.data(), port.length());
            dec.ProcessString((byte*)key.data(), key.length());
        }

        mContactList.insert(id, new Contact(id,
                                            name,
                                            deserializeStringList(hostList),
                                            qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(port.constData())),
                                            key,
                                            this));
    }
    QByteArray storedHash;
    if(!readDiskQuery.exec("SELECT hash FROM integrity;"))
        emit error(readDiskQuery.lastError().text());
    else{
        readDiskQuery.next();
        storedHash = readDiskQuery.value(0).toByteArray();
        if(storedHash != generateHash()){
            emit error(tr("Error: Contact database content corrupted"));
            qDeleteAll(mContactList);
            mContactList.clear();
        }
    }
    mDatabase.close();
}

void ContactDB::commitToDatabase(){
    openDB();
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption enc;
    if(!mFileKey->first.isEmpty()){
        enc.SetKeyWithIV((byte*)mFileKey->first.data(),
                         mFileKey->first.length(),
                         (byte*)mFileKey->second.data());
    }
    QSqlQuery writeDiskQuery(mDatabase);
    if(!writeDiskQuery.exec("DELETE FROM contacts"))
        emit error(writeDiskQuery.lastError().text());
    foreach(Contact *contact, mContactList){
        uint id = contact->getId();
        QByteArray name = contact->getName().toUtf8();
        QByteArray hostList = serializeStringList(contact->getHostsList());
        QByteArray port;
        QDataStream(&port, QIODevice::WriteOnly) << contact->getPort();
        QByteArray key = contact->getKey();

        if(!mFileKey->first.isEmpty()){
            enc.ProcessString((byte*)name.data(), name.length());
            enc.ProcessString((byte*)hostList.data(), hostList.length());
            enc.ProcessString((byte*)port.data(), port.length());
            enc.ProcessString((byte*)key.data(), key.length());
        }

        writeDiskQuery.prepare("INSERT INTO contacts(id,name,hosts,port,key) "
                      "VALUES (:id,:name,:hosts,:port,:key);");
        writeDiskQuery.bindValue(":id", id);
        writeDiskQuery.bindValue(":name", name);
        writeDiskQuery.bindValue(":hosts", hostList);
        writeDiskQuery.bindValue(":port", port);
        writeDiskQuery.bindValue(":key", key);
        if(!writeDiskQuery.exec())
            emit error(writeDiskQuery.lastError().text());
    }

    QSqlQuery updateHashQuery(mDatabase);
    if(updateHashQuery.exec("DELETE FROM integrity;")) //clear the table
        emit error(updateHashQuery.lastError().text());
    updateHashQuery.prepare("INSERT INTO integrity(hash) "
                            "VALUES (:hash);");
    updateHashQuery.bindValue(":hash", generateHash());
    if(!updateHashQuery.exec())
        emit error(updateHashQuery.lastError().text());
    mDatabase.close();
}

void ContactDB::openDB(){
    if(!QSqlDatabase::contains("diskDb")){
        mDatabase = QSqlDatabase::addDatabase("QSQLITE", "diskDb");
        mDatabase.setDatabaseName("contacts.db3");
    }
    if(!mDatabase.open())
        emit error(mDatabase.lastError().text());
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
