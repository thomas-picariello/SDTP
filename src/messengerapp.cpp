#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(NetworkManager *manager, QWidget* parent) :
    AbstractApp(manager, parent),
    ui(new Ui::MessengerApp)
{


    ui->setupUi(this);



    show();



}

void MessengerApp::addContact(NetworkManager *manager)
{

    m_AgentList.append(manager->getRootAgent());

}
