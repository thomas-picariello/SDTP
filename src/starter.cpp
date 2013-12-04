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
    *IP = settings->value("Contacts/"+nametouse+"/IP").toString();
    *port = settings->value("Contacts/"+nametouse+"/port").toString();
    *key = settings->value("Contacts/"+nametouse+"/key").toString();


}
Starter::~Starter()
{

}
