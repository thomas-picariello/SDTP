#include "pinger.h"

Pinger::Pinger(QObject *parent):
    QObject(parent),
    m_Link(NULL),
    m_Contact(NULL),
    m_HostIndex(0)
{
    connect(&m_ConnectTimer, SIGNAL(timeout()),
            this, SLOT(connectToNextHost()));
}

QString Pinger::getActiveHost() const{
    return m_ActiveHost;
}

void Pinger::setContact(Contact *contact){
    m_Contact = contact;
}

void Pinger::setLink(AbstractLink *link){
    m_Link = dynamic_cast<TcpLink*>(link);
    connect(m_Link, SIGNAL(connected()),
            this, SLOT(onConnected()));
}

bool Pinger::start(int delay){
    bool initialized = (m_Contact != 0 && m_Link != 0);
    if (initialized)
        m_ConnectTimer.start(delay*1000); //default 0s
    return initialized;
}

void Pinger::connectToNextHost(){
    m_HostIndex = (m_HostIndex+1) % m_Contact->getHostsList().length();
    m_Link->setHost(qMakePair(m_Contact->getHostsList().at(m_HostIndex), m_Contact->getPort()));

    if(m_HostIndex == m_Contact->getHostsList().length())
        m_ConnectTimer.setInterval(10000); //do a 10s pause at the end of the list
    else
        m_ConnectTimer.setInterval(1000); //otherwise 1s between pings
}

void Pinger::onConnected(){
    m_ConnectTimer.stop();
    m_ActiveHost = m_Link->getHost().first;
    emit connected();
}
