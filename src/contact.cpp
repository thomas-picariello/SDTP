#include "contact.h"

Contact::Contact(){
    mSettings.beginGroup("Contacts");
    mId = getNextAvailableID();
    mName = QString();
}
Contact::Contact(QString name){
    mSettings.beginGroup("Contacts");
    mName = name;
    int id = getNameID(mName);
    if(id != -1){ //contact exists, get the values
        mId = id;
        mName = name;
        QString idStr = mSettings.childGroups().first();
        mIp = mSettings.value(QString::number(mId) +"/ip").toString();
        mPort = mSettings.value(QString::number(mId) + "/port").toString();
        mKey = mSettings.value(QString::number(mId) + "/key").toByteArray();
    }
}
Contact::Contact(QString name, QString ip, QString port, QByteArray key){
    Contact();
    if(nameExists(name)){
        mId = getNextAvailableID();
        mName = name;
        mIp = ip;
        mPort = port;
        mKey = key;

        setName(mName);
        setIp(mIp);
        setPort(mPort);
        setKey(mKey);


    }
}

bool Contact::nameExists(QString name){
    int id = getNameID(name);
    if(id >= 0){
        return true;
    }else{
        return false;
    }
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
    if(!nameExists(name)){
        //name doesn't exists, set a new one
        mName = name;
        mSettings.setValue(QString::number(mId) + "/name", mName);
    }
}
void Contact::setIp(QString ip){
    if(!mName.isEmpty()){
        mIp = ip;
        mSettings.setValue(QString::number(mId) + "/ip", ip);
    }
}
void Contact::setPort(QString port){
    if(!mName.isEmpty()){
        mPort = port;
        mSettings.setValue(QString::number(mId) + "/port", port);
    }
}
void Contact::setKey(QByteArray key){
    if(!mName.isEmpty()){
        mKey = key;
        mSettings.setValue(QString::number(mId) + "/key", key);
    }
}
void Contact::remove(){
    if(!mName.isEmpty()){
        mSettings.remove(QString::number(mId));
    }
}

int Contact::getNextAvailableID(){
    QSettings settings;
    settings.beginGroup("Contacts");
    int higherId = 0 ;
    for(int i=0; i<settings.childGroups().size(); i++){
        int itemId = settings.childGroups().at(i).toInt();
        if(settings.childGroups().size() > 0 && itemId > higherId)
        {
            higherId = itemId;
        }
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
QStringList Contact::getAllNames(){
    QSettings settings;
    QStringList nameList;
    QString id;
    settings.beginGroup("Contacts");
    for(int i=0; i<settings.childGroups().size(); i++){
        id = settings.childGroups().at(i);
        nameList.append(settings.value(id + "/name").toString());
    }
    settings.endGroup();
    return nameList;
}

Contact::~Contact()
{
    mSettings.endGroup();
}
