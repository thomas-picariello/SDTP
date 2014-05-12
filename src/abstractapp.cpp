#include "abstractapp.h"

AbstractApp::AbstractApp(NetworkManager *manager, QWidget *parent) :QWidget(parent)
{

    m_AgentList->append(manager->getRootAgent());



}

void AbstractApp::dataToRead(QByteArray *data){

    m_Data = data;

}

AbstractApp::~AbstractApp(){


    delete m_AgentList,m_Data;
}
