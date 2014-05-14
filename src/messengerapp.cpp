#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(PacketAgent *agent, QWidget* parent) :
    AbstractApp(agent, parent),
    ui(new Ui::MessengerApp)
{


    ui->setupUi(this);
    show();

}

void MessengerApp::addContact(PacketAgent *agent)
{

    //m_AgentList.append(manager->getRootAgent());

}
