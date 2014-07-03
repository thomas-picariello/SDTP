#include "rootapp.h"

RootApp::RootApp(Contact* contact, QWidget *parent):
    AbstractApp(contact, parent)
{
    qDebug()<< "Root App started for" << m_ContactList.first()->getName();
    sendData(TCP, QByteArray("Hello from root !"));
}

void RootApp::readIncommingData(QByteArray &data){
    qDebug()<<"Incomming from root app:"<<data;
}

RootApp::~RootApp(){

}
