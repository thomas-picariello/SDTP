#include "tcplink.h"

TcpLink::TcpLink(QIODevice *parent):
    AbstractLink(parent),
    m_Socket(new QTcpSocket(this))
{
    connectSocketSignals();
}

TcpLink::TcpLink(const QString &host, quint16 port, QIODevice *parent):
    AbstractLink(parent),
    m_Socket(new QTcpSocket(this))
{
    connectSocketSignals();
    m_Socket->connectToHost(host, port);
}

TcpLink::TcpLink(QTcpSocket *socket, QIODevice *parent):
    AbstractLink(parent),
    m_Socket(socket)
{
    connectSocketSignals();
    m_State = Online;
    setOpenMode(ReadWrite);
    emit stateChanged(m_State);
    emit connected();
}

void TcpLink::connectSocketSignals(){
    connect(m_Socket, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    connect(m_Socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(internalStateChanged(QAbstractSocket::SocketState)));
    connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
}

void TcpLink::onConnectionError(QAbstractSocket::SocketError){
    emit disconnected(); //TODO: check if meaningfull
    emit error(m_Socket->errorString());
}

QPair<QString,quint16> TcpLink::getHost() const{
    return qMakePair(m_Socket->peerAddress().toString(), m_Socket->peerPort());
}

void TcpLink::setHost(QPair<QString, quint16> &host){
    m_Socket->abort();
    m_Socket->connectToHost(host.first, host.second);
}

void TcpLink::setSocket(QTcpSocket *socket){
    delete m_Socket;
    m_Socket = socket;
    connectSocketSignals();
    m_State = Online;
    setOpenMode(ReadWrite);
    emit stateChanged(m_State);
    emit connected();
}

void TcpLink::internalStateChanged(QAbstractSocket::SocketState state){
    switch(state){
    case QAbstractSocket::ConnectedState:
        m_State = Online;
        setOpenMode(ReadWrite);
        emit stateChanged(m_State);
        emit connected();
        break;
    default:
        m_State = Offline;
        setOpenMode(NotOpen);
        emit stateChanged(m_State);
        emit disconnected();
        break;
    }
}

qint64 TcpLink::readData(char *data, qint64 maxSize){
    return m_Socket->read(data, maxSize);
}

qint64 TcpLink::writeData(const char *data, qint64 size){
    return m_Socket->write(data, size);
}

TcpLink::~TcpLink(){
    delete m_Socket;
}
