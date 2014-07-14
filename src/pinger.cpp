#include "pinger.h"

Pinger::Pinger(Contact* contact, QObject *parent):
    QObject(parent),
    m_contact(contact),
    m_socket(new QTcpSocket),
    m_hostIndex(0)
{
    connect(&m_connectTimer, &QTimer::timeout,
            this, &Pinger::connectToNextHost);
    connect(m_socket, &QTcpSocket::connected,
            this, &Pinger::onConnected);
}

QTcpSocket* Pinger::getSocket(){
    return m_socket;
}

bool Pinger::hasHost(QString &host) const{
    foreach(QString contactHost, m_contact->getHostsList())
        if(host == contactHost)
            return true;
    return false;
}

Contact* Pinger::getContact(){
    return m_contact;
}

void Pinger::setSocket(QTcpSocket *socket){
    m_socket = socket;
}

void Pinger::start(int delay){
    m_connectTimer.start(delay*1000); //default 0s
}

void Pinger::stop(){
    m_connectTimer.stop();
    m_hostIndex = 0;
}

void Pinger::connectToNextHost(){
    m_hostIndex = (m_hostIndex+1) % m_contact->getHostsList().length();
    m_socket->abort();
    m_socket->connectToHost(m_contact->getHostsList().at(m_hostIndex), m_contact->getPort());

    if(m_hostIndex == m_contact->getHostsList().length())
        m_connectTimer.setInterval(10000); //do a 10s pause at the end of the list
    else
        m_connectTimer.setInterval(1000); //otherwise 1s between pings
}

void Pinger::onConnected(){
    m_connectTimer.stop();
    emit connected(m_socket);
}
