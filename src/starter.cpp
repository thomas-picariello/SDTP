#include "starter.h"

Starter::Starter()
{
    settings = new QSettings;
    messenger = new Messenger;

    name = new QString;
    IP   = new QString;
    port = new qint16;
    key  = new QString;

    socket = new QTcpSocket;


}
void Starter::opennewConnection(QString nametouse)
{
    qDebug()<<"opennewConnection() launched succesfully with : "+nametouse;

    *name = nametouse;
    qDebug()<<"1";
    *IP = settings->value("Contacts/"+nametouse+"/IP").toString();
    qDebug()<<"2";
    *port = settings->value("Contacts/"+nametouse+"/port").toInt();
    qDebug()<<"3";
    *key = settings->value("Contacts/"+nametouse+"/key").toString();

    qDebug()<<"IP";
    qDebug()<<*IP;
    qDebug()<<"port";
    qDebug()<<*port;
    qDebug()<<"key";
    qDebug()<<*key;



    socket->abort();
    socket->connectToHost(*IP,*port,QIODevice::ReadWrite,QAbstractSocket::AnyIPProtocol);

    if (socket->waitForConnected( 5000 ))
    {

        messenger->displaymsg("Successfully connected to"+*IP);
        qDebug()<<"connecttohost seems to have worked correctly ... ?";
        if (socket->isWritable())socket->write("Hello there !");
    }
    else qDebug()<<socket->error();


    socket->abort();


}
Starter::~Starter()
{

}
