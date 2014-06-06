#include "pinger.h"

Pinger::Pinger(QObject *parent):
    QObject(parent),
    m_HostIndex(0)
{
    connect(&m_ConnectTimer, SIGNAL(timeout()),
            this, SLOT(connectToNextHost()));
}

Pinger::Pinger(Contact *contact, TcpLink *link, QObject *parent) :
    QObject(parent),
    m_Contact(contact),
    m_Link(link),
    m_HostIndex(0)
{
    connect(&m_ConnectTimer, SIGNAL(timeout()),
            this, SLOT(connectToNextHost()));
    start();
}

QString Pinger::getActiveHost() const{
    return m_ActiveHost;
}

void Pinger::setContact(Contact *contact){
    m_Contact = contact;
}

void Pinger::setLink(AbstractLink *link){
    m_Link = dynamic_cast<TcpLink*>(link);
}

bool Pinger::start(){
    bool initialized = (m_Contact != 0 && m_Link != 0);
    if (initialized)
        m_ConnectTimer.start(0); //immediatly start the first ping
    return initialized;
}

void Pinger::connectToNextHost(){
    if(m_Link->state() == AbstractLink::Offline){
        m_HostIndex = (m_HostIndex+1) % m_Contact->getHostsList().length();
        m_Link->setHost(qMakePair(m_Contact->getHostsList().at(m_HostIndex), m_Contact->getPort()));

        if(m_HostIndex == m_Contact->getHostsList().length())
            m_ConnectTimer.setInterval(10000); //do a 10s pause at the end of the list
        else
            m_ConnectTimer.setInterval(1000); //otherwise 1s between pings
    }else{
        m_ConnectTimer.stop();
        m_ActiveHost = m_Link->getHost().first;
        emit connected(m_ActiveHost);
    }
}
