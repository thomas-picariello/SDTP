#include "tcplink.h"

TcpLink::TcpLink(QIODevice *parent):
    AbstractLink(parent),
    m_socket(new QTcpSocket(this))
{
    connectSocketSignals();
}

TcpLink::TcpLink(const QString &host, quint16 port, QIODevice *parent):
    AbstractLink(parent),
    m_socket(new QTcpSocket(this))
{
    connectSocketSignals();
    m_socket->connectToHost(host, port);
}

TcpLink::TcpLink(QTcpSocket *socket, QIODevice *parent):
    AbstractLink(parent)
{
    setSocket(socket);
    connectSocketSignals();
}

void TcpLink::connectSocketSignals(){
    connect(m_socket, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(internalStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
}

qint64 TcpLink::bytesAvailable() const{
    return m_socket->bytesAvailable();
}

QString TcpLink::getHost() const{
    return m_socket->peerAddress().toString();
}

quint16 TcpLink::getPort() const{
    return m_socket->peerPort();
}

void TcpLink::setHost(const QString &host, const quint16 port){
    m_socket->abort();
    m_socket->connectToHost(host, port);
}

void TcpLink::setSocket(QTcpSocket *socket){
    delete m_socket;
    m_socket = socket;
    m_socket->setParent(this);
    connectSocketSignals();
    m_state = Online;
    setOpenMode(socket->openMode());
    emit openModeChanged(openMode());
    emit stateChanged(m_state);
    emit connected();
}

void TcpLink::internalStateChanged(QAbstractSocket::SocketState state){
    switch(state){
    case QAbstractSocket::ConnectedState:
        m_state = Online;
        setOpenMode(ReadWrite);
        emit openModeChanged(ReadWrite);
        emit stateChanged(m_state);
        emit connected();
        break;
    default:
        m_state = Offline;
        setOpenMode(NotOpen);
        emit openModeChanged(NotOpen);
        emit stateChanged(m_state);
        emit disconnected();
        break;
    }
}

void TcpLink::onConnectionError(QAbstractSocket::SocketError){
    emit error(m_socket->errorString());
    emit disconnected();
}

qint64 TcpLink::readData(char *data, qint64 maxSize){
    return m_socket->read(data, maxSize);
}

qint64 TcpLink::writeData(const char *data, qint64 size){
    return m_socket->write(data, size);
}

TcpLink::~TcpLink(){
    delete m_socket;
}
