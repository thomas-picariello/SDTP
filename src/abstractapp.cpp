#include "abstractapp.h"

AbstractApp::AbstractApp(QWidget *parent):
    QWidget(parent)
{}

AbstractApp::AbstractApp(QList<Contact*> contactList, QWidget *parent):
    QWidget(parent)
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

AbstractApp::AppUID::AppUID(AppType typeId, quint16 instanceId):
    type(typeId),
    instanceID(instanceId)
{}

bool AbstractApp::AppUID::operator<(const AppUID &second) const{
    if(type == second.type)
        return (instanceID < second.instanceID);
    return (type < second.type);
}

QDataStream &operator<<(QDataStream &out, const AbstractApp::AppUID& appUID){
    out << (quint8)appUID.type
        << appUID.instanceID;
    return out;
}

QDataStream &operator>>(QDataStream &in, AbstractApp::AppUID& appUID){
    quint8 _type;
    quint16 _instanceID;
    in >> _type;
    in >> _instanceID;
    appUID = AbstractApp::AppUID(static_cast<AppType>(_type), _instanceID); //TODO: improve with moc (if possible)
    return in;
}
