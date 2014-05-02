#include "abstractapp.h"

AbstractApp::AbstractApp(NetworkManager *manager)
{

    m_Root = manager->getRootAgent();

//    m_Root->login(this);


}

void AbstractApp::dataToRead(QByteArray *data){

    m_Data = data;

}

AbstractApp::~AbstractApp(){

//    m_Root->logout(this);

    delete m_Root,m_Data;
}
