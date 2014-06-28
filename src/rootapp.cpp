#include "rootapp.h"

RootApp::RootApp(QList<Contact*> contactList, QWidget *parent):
    AbstractApp(contactList, parent)
{
    qDebug()<< "Root App started for" << contactList.first()->getName();
}

void RootApp::readIncommingData(QByteArray &data){

}

RootApp::~RootApp(){

}
