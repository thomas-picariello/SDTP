#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(NetworkManager *manager, QWidget* parent) :
    AbstractApp(manager, parent)
{

    ui->setupUi(this);


}

void MessengerApp::addContact(NetworkManager *manager)
{

    m_AgentList->append(manager->getRootAgent());

}
