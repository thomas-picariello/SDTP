#include "contactfactory.h"

Contact* ContactFactory::findById(int id, QObject *parent){
    quint16 cPort;
    QString cId, cName, cHost;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        if(cId.toInt() == id){
            cName = settings.value(cId + "/name").toString();
            cHost = settings.value(cId + "/host").toString();
            cPort = settings.value(cId + "/port").toUInt();
            cKey = settings.value(cId + "/key").toByteArray();
            return new Contact(cId.toInt(),cName, cHost, cPort, cKey, parent);
        }
    }
    settings.endGroup();
    return NULL;
}

QList<Contact*> ContactFactory::findByName(QString name, QObject *parent){
    QList<Contact*> matchList;
    QString cId, cName;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cName = settings.value(cId + "/name").toString();
        if(cName.compare(name) == 0){
            QString cHost = settings.value(cId + "/host").toString();
            quint16 cPort = settings.value(cId + "/port").toUInt();
            QByteArray cKey = settings.value(cId + "/key").toByteArray();
            matchList.append(new Contact(cId.toInt(),cName, cHost, cPort, cKey, parent));
        }
    }
    settings.endGroup();
    return matchList;
}

QList<Contact*> ContactFactory::findByHost(QString host, QObject *parent){
    QList<Contact*> matchList;
    QString cId;
    QString cHost;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cHost = settings.value(cId + "/host").toString();
        if(cHost.compare(host) == 0){
            QString cName = settings.value(cId + "/name").toString();
            quint16 cPort = settings.value(cId + "/port").toUInt();
            QByteArray cKey = settings.value(cId + "/key").toByteArray();
            matchList.append(new Contact(cId.toInt(),cName, cHost, cPort, cKey, parent));
        }
    }
    settings.endGroup();
    return matchList;
}

Contact* ContactFactory::findByKey(QByteArray key, QObject *parent){
    quint16 cPort;
    QString cId, cName, cHost;
    QByteArray cKey;
    QSettings settings;
    settings.beginGroup("Contacts");
    foreach(cId, settings.childGroups()){
        cKey = settings.value(cId + "/key").toByteArray();
        if(cKey == key){ //TODO: test
            cName = settings.value(cId + "/name").toString();
            cHost = settings.value(cId + "/host").toString();
            cPort = settings.value(cId + "/port").toUInt();
            return new Contact(cId.toInt(),cName, cHost, cPort, cKey, parent);
        }
    }
    settings.endGroup();
    return NULL;
}

QList<Contact*> ContactFactory::getContactList(QObject *parent){
    QSettings settings;
    QList<Contact*> contactList;
    settings.beginGroup("Contacts");
    QStringList idList = settings.childGroups();
    foreach(QString id, idList)
        contactList.append(ContactFactory::findById(id.toInt(), parent));
    settings.endGroup();
    return contactList;
}
