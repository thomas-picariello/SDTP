#include "abstractapp.h"

AbstractApp::AbstractApp(NetworkManager *manager)
{

    m_Root = manager->getRootAgent();



}

void AbstractApp::dataToRead(QByteArray *data){

    m_Data = data;

}

AbstractApp::~AbstractApp(){


    delete m_Root,m_Data;
}
