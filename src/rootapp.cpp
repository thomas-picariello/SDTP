#include "rootapp.h"

RootApp::RootApp(QWidget *parent):
    AbstractApp(parent)
{

}

RootApp::RootApp(QList<Contact*> contactList, QWidget *parent):
    AbstractApp(contactList, parent)
{

}

void RootApp::readIncommingData(QByteArray &data){

}

RootApp::~RootApp(){
    AbstractApp::~AbstractApp();
}
