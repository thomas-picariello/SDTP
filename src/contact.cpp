#include "contact.h"

Contact::Contact(QObject *parent):
    QObject(parent),
    m_id(0),
    m_port(0),
    m_status(Offline)
{}

Contact::Contact(int id, QString name, QStringList hostsList, quint16 port, QByteArray key, QObject *parent):
    QObject(parent),
    m_id(id),
    m_name(name),
    m_hostsList(hostsList),
    m_port(port),
    m_key(key),
    m_status(Offline)
{}

QString Contact::getActiveHost() const{
    return m_activeHost;
}

QStringList Contact::getHostsList() const{
    return m_hostsList;
}

uint Contact::getId() const{
    return m_id;
}

QByteArray Contact::getKey() const{
    return m_key;
}

QString Contact::getName() const{
    return m_name;
}

quint16 Contact::getPort() const{
    return m_port;
}

Contact::Status Contact::getStatus() const{
    return m_status;
}

QStringList* Contact::hostsList(){
    return &m_hostsList;
}

void Contact::setActiveHost(QString& activeHost){
    m_activeHost = activeHost;
}

void Contact::setHostsList(QStringList hostsList){
    m_hostsList = hostsList;
}

void Contact::setId(uint id){
    m_id = id;
}

void Contact::setKey(QByteArray key){
    m_key = key;
}

void Contact::setName(QString name){
    m_name = name;
}

void Contact::setPort(quint16 port){
    m_port = port;
}

void Contact::setStatus(Contact::Status status){
    m_status = status;
    emit statusChanged();
}

bool Contact::operator ==(const Contact &other) const{
    return (m_id == other.getId());
}

bool Contact::operator <(const Contact &other) const{
    return (m_id < other.getId());
}
