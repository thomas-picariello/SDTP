#include "contactdb.h"

ContactDB::ContactDB(QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent), m_fileKey(fileKey)
{
    initDbTables();
    importDatabase();
}

ContactDB::~ContactDB(){
    qDeleteAll(m_contactList);
}

void ContactDB::updateFileKey(){
    commitToDatabase();
}

void ContactDB::erase(int id){
    delete m_contactList.value(id);
    m_contactList.remove(id);
    commitToDatabase();
    emit contactEvent(id, ContactDeleted);
}

Contact* const ContactDB::findById(int id){
    return m_contactList.value(id, NULL);
}

Contact* const ContactDB::findByKey(QByteArray key){
    foreach(Contact* contact, m_contactList){
        if(contact->getKey() == key)
            return contact;
    }
    return NULL;
}

QList<Contact*> ContactDB::getAllContacts() const{
    return m_contactList.values();
}

uint ContactDB::save(Contact *contact){
    uint id;
    if(contact->getId() == 0){  //new contact
        id = getNextAvailableId();
        contact->setParent(this);
        contact->setId(id);
        m_contactList.insert(id, contact);
        emit contactEvent(contact->getId(), ContactAdded);
    }else{
        id = contact->getId();
        emit contactEvent(contact->getId(), ContactEdited);
    }
    commitToDatabase();
    return id;
}

QByteArray ContactDB::generateHash(){
    QByteArray content;
    QDataStream contentStream(&content, QIODevice::WriteOnly);
    foreach(Contact *contact, m_contactList){
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
    QSqlQuery query(m_database);
    QString createContactTableQuery = "CREATE TABLE IF NOT EXISTS contacts("
                                           "id      INTEGER PRIMARY KEY, "
                                           "name    BLOB, "
                                           "hosts   BLOB, "
                                           "port    INTEGER, "
                                           "key     BLOB "
                                       ");";
    if(!query.exec(createContactTableQuery))
        emit error(query.lastError().text());

    QString createIntegrityTableQuery = "CREATE TABLE IF NOT EXISTS integrity("
                                            "hash   BLOB"
                                        ");";
    if(query.exec(createIntegrityTableQuery))
        emit error(query.lastError().text());
    m_database.close();
}

uint ContactDB::getNextAvailableId(){
    uint id = 1; //id start at 1 in DB (-> min. id = 1)
    QList<uint> idList;
    foreach(Contact* contact, m_contactList)
        idList << contact->getId();
    while(idList.contains(id))
        id++;
    return id;
}

void ContactDB::importDatabase(){
    openDB();
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption dec;
    if(!m_fileKey->first.isEmpty()){
        dec.SetKeyWithIV((byte*)m_fileKey->first.data(),
                         m_fileKey->first.length(),
                         (byte*)m_fileKey->second.data());
    }
    QSqlQuery readDiskQuery(m_database);
    readDiskQuery.exec("SELECT id,name,hosts,port,key FROM contacts;");
    while(readDiskQuery.next()){
        uint id = readDiskQuery.value(0).toUInt();                  //id
        QByteArray name = readDiskQuery.value(1).toByteArray();     //name
        QByteArray hostList = readDiskQuery.value(2).toByteArray(); //serialized host list
        QByteArray port = readDiskQuery.value(3).toByteArray();     //port
        QByteArray key = readDiskQuery.value(4).toByteArray();      //key

        if(!m_fileKey->first.isEmpty()){
            dec.ProcessString((byte*)name.data(), name.length());
            dec.ProcessString((byte*)hostList.data(), hostList.length());
            dec.ProcessString((byte*)port.data(), port.length());
            dec.ProcessString((byte*)key.data(), key.length());
        }

        m_contactList.insert(id, new Contact(id,
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
            qDeleteAll(m_contactList);
            m_contactList.clear();
        }
    }
    m_database.close();
}

void ContactDB::commitToDatabase(){
    openDB();
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption enc;
    if(!m_fileKey->first.isEmpty()){
        enc.SetKeyWithIV((byte*)m_fileKey->first.data(),
                         m_fileKey->first.length(),
                         (byte*)m_fileKey->second.data());
    }
    QSqlQuery query(m_database);
    if(!query.exec("DELETE FROM contacts"))
        emit error(query.lastError().text());
    foreach(Contact *contact, m_contactList){
        uint id = contact->getId();
        QByteArray name = contact->getName().toUtf8();
        QByteArray hostList = serializeStringList(contact->getHostsList());
        QByteArray port;
        QDataStream(&port, QIODevice::WriteOnly) << contact->getPort();
        QByteArray key = contact->getKey();

        if(!m_fileKey->first.isEmpty()){
            enc.ProcessString((byte*)name.data(), name.length());
            enc.ProcessString((byte*)hostList.data(), hostList.length());
            enc.ProcessString((byte*)port.data(), port.length());
            enc.ProcessString((byte*)key.data(), key.length());
        }

        query.prepare("INSERT INTO contacts(id,name,hosts,port,key) "
                      "VALUES (:id,:name,:hosts,:port,:key);");
        query.bindValue(":id", id);
        query.bindValue(":name", name);
        query.bindValue(":hosts", hostList);
        query.bindValue(":port", port);
        query.bindValue(":key", key);
        if(!query.exec())
            emit error(query.lastError().text());
    }

    QSqlQuery updateHashQuery(m_database);
    if(updateHashQuery.exec("DELETE FROM integrity;")) //clear the table
        emit error(updateHashQuery.lastError().text());
    updateHashQuery.prepare("INSERT INTO integrity(hash) "
                            "VALUES (:hash);");
    updateHashQuery.bindValue(":hash", generateHash());
    if(!updateHashQuery.exec())
        emit error(updateHashQuery.lastError().text());
    m_database.close();
}

void ContactDB::openDB(){
    if(!QSqlDatabase::contains("contactDB")){
        m_database = QSqlDatabase::addDatabase("QSQLITE", "contactDB");
        m_database.setDatabaseName("contacts.db3");
    }else{
        m_database = QSqlDatabase::database("contactDB");
    }
    if(!m_database.open())
        emit error(m_database.lastError().text());
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
