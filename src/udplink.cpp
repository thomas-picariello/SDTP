#include "udplink.h"

UdpLink::UdpLink(const QString &host, quint16 hostPort, quint16 listenPort, QIODevice *parent):
    AbstractLink(parent),
    m_inputSocket(new QUdpSocket(this)),
    m_outputSocket(new QUdpSocket(this))
{
    connect(m_inputSocket, &QUdpSocket::readyRead,
            this, &UdpLink::readyRead);
    connect(m_inputSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_inputSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
    m_inputSocket->bind(QHostAddress::LocalHost, listenPort);
    m_outputSocket->connectToHost(host, hostPort);

    m_state = Online;
    setOpenMode(ReadWrite);
    emit openModeChanged(ReadWrite);
    emit stateChanged(m_state);
}

qint64 UdpLink::bytesAvailable() const{
    return m_inputSocket->bytesAvailable();
}

QString UdpLink::getHost() const{
    return m_outputSocket->peerAddress().toString();
}

quint16 UdpLink::getPort() const{
    return m_outputSocket->peerPort();
}

void UdpLink::setHost(const QString &host, const quint16 port){
    m_outputSocket->abort();
    m_outputSocket->connectToHost(host, port);
}

void UdpLink::onConnectionError(QAbstractSocket::SocketError){
    emit error(dynamic_cast<QAbstractSocket*>(sender())->errorString());
}

qint64 UdpLink::readData(char *data, qint64 maxSize){
    return m_inputSocket->read(data, maxSize);
}

qint64 UdpLink::writeData(const char *data, qint64 size){
    return m_outputSocket->write(data, size);
}

UdpLink::~UdpLink(){
    delete m_inputSocket;
    delete m_outputSocket;
}
