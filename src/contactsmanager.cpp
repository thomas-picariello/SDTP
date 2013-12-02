#include "contactsmanager.h"
#include <QDebug>

ContactsManager::ContactsManager()
{

    Contactwindow = new QWidget();
    About = new QPushButton("AboutQt", Contactwindow);
    Contactwindow->resize(300,100);


    connect(About,SIGNAL(clicked()),qApp,SLOT(aboutQt()));


}
void ContactsManager::contactsWindow()
{
    qDebug()<<"contactsWindow !";

    Contactwindow->show();


}
ContactsManager::~ContactsManager(void)
{

}
