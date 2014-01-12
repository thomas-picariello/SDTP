#include "contact.h"

Contact::Contact(QString name, QString ip, QString port, QByteArray key){
    mId = getNextAvailableID();
    mName = name;
    mIp = ip;
    mPort = port;
    mKey = key;
}

Contact Contact::findById(int id){
    Contact contact;
    QSettings settings;
    settings.beginGroup("Contacts");
    for(int i=0; i<settings.childGroups().size(); i++){
        QString actualId = settings.childGroups().at(i);
        if(actualId.toInt() == id){
            contact.setId(actualId.toInt());
            contact.setName(settings.value(actualId + "/name").toString());
            contact.setIp(settings.value(actualId + "/ip").toString());
            contact.setPort(settings.value(actualId + "/port").toString());
            contact.setKey(settings.value(actualId + "/key").toByteArray());
            break; //should exit the for loop...
        }
    }
    settings.endGroup();
    return contact;
}

Contact Contact::findByName(QString name){
    Contact contact;
    QSettings settings;
    settings.beginGroup("Contacts");
    for(int i=0; i<settings.childGroups().size(); i++){
        QString id = settings.childGroups().at(i);
        QString actualName = settings.value(id + "/name").toString();
        if(name.compare(actualName) == 0){
            contact.setId(id.toInt());
            contact.setName(actualName);
            contact.setIp(settings.value(id + "/ip").toString());
            contact.setPort(settings.value(id + "/port").toString());
            contact.setKey(settings.value(id + "/key").toByteArray());
            break; //should exit the for loop...
        }
    }
    settings.endGroup();
    return contact;
}

Contact Contact::findByIp(QString ip){
    Contact contact;
    QSettings settings;
    settings.beginGroup("Contacts");
    for(int i=0; i<settings.childGroups().size(); i++){
        QString id = settings.childGroups().at(i);
        QString actualIp = settings.value(id + "/ip").toString();
        if(ip.compare(actualIp) == 0){
            contact.setId(id.toInt());
            contact.setName(settings.value(id + "/name").toString());
            contact.setIp(actualIp);
            contact.setPort(settings.value(id + "/port").toString());
            contact.setKey(settings.value(id + "/key").toByteArray());
            break;
        }
    }
    settings.endGroup();
    return contact;
}

Contact Contact::findByKey(QByteArray key){
    Contact contact;
    QSettings settings;
    settings.beginGroup("Contacts");
    for(int i=0; i<settings.childGroups().size(); i++){
        QString id = settings.childGroups().at(i);
        QByteArray actualKey = settings.value(id + "/key").toByteArray();
        if(actualKey == key){
            contact.setId(id.toInt());
            contact.setName(settings.value(id + "/name").toString());
            contact.setIp(settings.value(id + "/ip").toString());
            contact.setPort(settings.value(id + "/port").toString());
            contact.setKey(actualKey);
            break;
        }
    }
    settings.endGroup();
    return contact;
}

QList<Contact> Contact::getContactList(){
    QSettings settings;
    QList<Contact> contactList;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    foreach(QString id, idList)
        contactList.append(Contact::findById(id.toInt()));

    settings.endGroup();
    return contactList;
}

void Contact::setId(int id){
    mId = id;
}

QString Contact::getName() const{
    return mName;
}

QString Contact::getIp() const{
    return mIp;
}

QString Contact::getPort() const{
    return mPort;
}

QByteArray Contact::getKey() const{
    return mKey;
}

void Contact::setName(QString name){
    mName = name;
}

void Contact::setIp(QString ip){
    mIp = ip;
}

void Contact::setPort(QString port){
    mPort = port;
}

void Contact::setKey(QByteArray key){
    mKey = key;
}

void Contact::save(){
    QSettings settings;
    settings.beginGroup("Contacts");
    settings.setValue(QString::number(mId) + "/name", mName);
    settings.setValue(QString::number(mId) + "/ip", mIp);
    settings.setValue(QString::number(mId) + "/port", mPort);
    settings.setValue(QString::number(mId) + "/key", mKey);
    settings.endGroup();
}

void Contact::erase(){
    QSettings settings;
    settings.beginGroup("Contacts");
    if(settings.childGroups().contains(QString::number(mId)))
        settings.remove(QString::number(mId));
    settings.endGroup();
}

//TODO: to improve
int Contact::getNextAvailableID(){
    QSettings settings;
    settings.beginGroup("Contacts");
    int higherId = 0 ;
    for(int i=0; i<settings.childGroups().size(); i++){
        int itemId = settings.childGroups().at(i).toInt();
        if(settings.childGroups().size() > 0 && itemId > higherId)
            higherId = itemId;
    }
    return higherId+1;
    settings.endGroup();
}

int Contact::getNameID(QString name){
    QSettings settings;
    settings.beginGroup("Contacts");
    int id = -1;
    for(int i=0; i<settings.childGroups().size(); i++){
        id = settings.childGroups().at(i).toInt();
        QString actualName = settings.value(QString::number(id) + "/name").toString();
        if(actualName.compare(name) == 0){
            settings.endGroup();
            return id;
        }
    }
    settings.endGroup();
    return -1;
}
