#include "abstractapp.h"

AbstractApp::AbstractApp(NetworkManager *manager, QWidget *parent):
    QWidget(parent),
    m_Data(NULL)
{

    PacketAgent* agent = manager->getRootAgent();
    m_AgentList.append(agent);


}

void AbstractApp::dataToRead(QByteArray *data){

    m_Data = data;

}

AbstractApp::~AbstractApp(){


    delete m_Data;
}
