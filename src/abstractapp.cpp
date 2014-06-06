#include "abstractapp.h"

AbstractApp::AbstractApp(ContactDB *contactDB, QWidget *parent):
    QWidget(parent),
    m_ContactDB(contactDB)
{}

AbstractApp::AbstractApp(QList<Contact*> contactList, ContactDB *contactDB, QWidget *parent):
    QWidget(parent),
    m_ContactDB(contactDB)
{
    m_ContactList.append(contactList);
    foreach(Contact *contact, m_ContactList)
        contact->setParent(this);
}

AbstractApp::~AbstractApp(){
    qDeleteAll(m_ContactList);
}

AbstractApp::AppUID::AppUID():
    type(Root),
    instanceID(0)
{}

AbstractApp::AppUID::AppUID(AppType typeId, uint instanceId):
    type(typeId),
    instanceID(instanceId)
{}

bool AbstractApp::AppUID::operator<(const AppUID &second) const{
    if(type == second.type)
        return (instanceID < second.instanceID);
    return (type < second.type);
}
