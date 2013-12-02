#include "contactsmanager.h"
#include <QDebug>

ContactsManager::ContactsManager()
{

    Contactwindow = new QWidget();
    Quitter = new QPushButton("Quitter", Contactwindow);


    connect(Quitter,SIGNAL(clicked()),qApp,SLOT(closeAllWindows()));


}
void ContactsManager::contactsWindow()
{
    qDebug()<<"contactsWindow !";

    Contactwindow->show();


}
ContactsManager::~ContactsManager(void)
{

}
