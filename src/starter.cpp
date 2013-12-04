#include "starter.h"

Starter::Starter()
{
    settings = new QSettings;
    messenger = new Messenger;

    name = new QString;
    IP   = new QString;
    port = new QString;
    key  = new QString;
}
void Starter::opennewConnection(QString nametouse)
{
    qDebug()<<"opennewConnection() launched succesfully with : "+nametouse;

    *name = nametouse;
    qDebug()<<"1";
    *IP = settings->value("Contacts/"+nametouse+"/IP").toString();
    qDebug()<<"2";
    *port = settings->value("Contacts/"+nametouse+"/port").toString();
    qDebug()<<"3";
    *key = settings->value("Contacts/"+nametouse+"/key").toString();

    qDebug()<<"IP";
    qDebug()<<*IP;
    qDebug()<<"port";
    qDebug()<<*port;
    qDebug()<<"key";
    qDebug()<<*key;



    socket = new QTcpSocket();


}
Starter::~Starter()
{

}
