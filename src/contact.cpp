#include "contact.h"

Contact::Contact(int id, QString name, QString hostName, QHostAddress ip, quint16 port, QByteArray key){
    flag_isResolving = false;
    mId = id;
    mName = name;
    mHostName = hostName;
    mIp = ip;
    mPort = port;
    mKey = key;
}

int Contact::getId() const{
    return mId;
}

QString Contact::getName() const{
    return mName;
}

QString Contact::getHostName() const{
    return mHostName;
}

QHostAddress Contact::getIpAddress() const{
    return mIp;
}

quint16 Contact::getPort() const{
    return mPort;
}

QByteArray Contact::getKey() const{
    return mKey;
}

bool Contact::isResolving() const{
    return flag_isResolving;
}

void Contact::setId(int id){
    mId = id;
}

void Contact::setName(QString name){
    mName = name;
}

void Contact::setHostName(QString hostName){
    flag_isResolving = true;
    QHostInfo::lookupHost(hostName,
                          this, SLOT(onResolve(QHostInfo)));
    mHostName = hostName;
}

void Contact::setIpAddress(QHostAddress ip){
    flag_isResolving = true;
    QHostInfo::lookupHost(ip.toString(),
                          this, SLOT(onResolve(QHostInfo)));
    mIp = ip;
}

void Contact::setPort(quint16 port){
    mPort = port;
}

void Contact::setKey(QByteArray key){
    mKey = key;
}

void Contact::save(){
    QString id = QString::number(mId);
    QSettings settings;
    settings.beginGroup("Contacts");
    settings.setValue(id + "/name", mName);
    settings.setValue(id + "/hname", mHostName);
    settings.setValue(id + "/ip", mIp.toString());
    settings.setValue(id + "/port", QString::number(mPort));
    settings.setValue(id + "/key", mKey);
    settings.endGroup();
}

void Contact::erase(){
    QSettings settings;
    settings.beginGroup("Contacts");
    if(settings.childGroups().contains(QString::number(mId)))
        settings.remove(QString::number(mId));
    settings.endGroup();
}

void Contact::onResolve(QHostInfo hostInfo){
    if(hostInfo.error() != QHostInfo::NoError){
        qDebug()<<hostInfo.errorString();
        emit resolveResult(hostInfo.errorString());
    }else{
        if(mHostName.size()==0)
            mHostName = hostInfo.hostName();
        if(mIp.isNull())
            mIp = hostInfo.addresses().first();
        emit resolveResult(mHostName+":"+mIp.toString());
    }
    flag_isResolving = false;
}

int Contact::getNextAvailableID(){
    QSettings settings;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    int freeId = 0;
    while(idList.contains(QString::number(freeId))){
        freeId++;
    }
    return freeId;
    settings.endGroup();
}
