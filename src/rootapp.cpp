#include "rootapp.h"

RootApp::RootApp(ContactDB *contactDB, QWidget *parent):
    AbstractApp(contactDB, parent)
{

}

RootApp::RootApp(QList<Contact*> contactList, ContactDB *contactDB, QWidget *parent):
    AbstractApp(contactList, contactDB, parent)
{

}

void RootApp::dataToRead(QByteArray data){

}

RootApp::~RootApp(){
    AbstractApp::~AbstractApp();
}
