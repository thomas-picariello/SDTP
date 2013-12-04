#include "contact.h"

Contact::Contact()
{
    QSettings settings;
    names = new QList<QString>;
    name = new QString;
    IP = new QString;
    port = new QString;
    key = new QByteArray;


    settings.setValue("Contacts/Default/name","Default");
    settings.setValue("Contacts/Default/IP","127.0.0.1");
    settings.setValue("Contacts/Default/port","80");
    settings.setValue("Contacts/Default/key","lalalalalala");


    *name = settings.value("Contacts/Default/name").toString();
    *IP = settings.value("Contacts/Default/IP").toString();
    *port = settings.value("Contacts/Default/port").toString();
    *key = settings.value("Contacts/Default/key").toByteArray();




}

QList<QString> Contact::getnames()
{
    settings->beginGroup("Contacts");
    qDebug()<<settings->childGroups();
    names->append(settings->childGroups());

    settings->endGroup();
    return *names;
}
QString Contact::getIP(QString name)
{
    *IP = settings->value("Contacts/"+name+"/IP").toString();

    qDebug()<<"IP is : "+*IP;

    return *IP;
}
QString Contact::getport(QString name)
{
    *port = settings->value("Contacts/"+name+"/port").toString();

    qDebug()<<"port is : "+*port;

    return *port;

}
QByteArray Contact::getkey(QString name)
{

    *key = settings->value("Contacts/"+name+"/key").toByteArray();

    qDebug()<<"key is : "+*key;

    return *key;
}
void Contact::setname(QString name)
{

    qDebug()<<"name set as : "+name;

    settings->setValue("Contacts/"+name+"/name",name);

}
void Contact::setIP(QString name, QString IP)
{
    settings->setValue("Contacts/"+name+"/IP", IP);

    qDebug()<<"IP from "+name+"set to : "+IP;

}
void Contact::setport(QString name, QString port)
{
    settings->setValue("Contacts/"+name+"/port", port);

    qDebug()<<"port from "+name+"set to : "+port;
}
void Contact::setkey(QString name, QByteArray key)
{
    settings->setValue("Contacts/"+name+"/key", key);

    qDebug()<<"key from "+name+"set to : "+key;

}
Contact::~Contact()
{

}
