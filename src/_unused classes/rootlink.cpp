#include "rootlink.h"

RootLink::RootLink(Contact *contact, QObject *parent):
    AbstractLink(parent),
    m_Contact(contact),
    m_Timer(new QTimer(this))
{
    m_Socket = new QTcpSocket();
    //mAgent = new PacketAgent(QPair<QByteArray,QByteArray>(m_Contact->getKey(),"000"));
    m_HostListIndex = m_Contact->hostsList()->length();

    connect(m_Socket,SIGNAL(connected()),
            this,SLOT(onConnected()));
    connect(m_Socket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_Timer, SIGNAL(timeout()),
            this, SLOT(tryConnect()));

    m_Timer->start(5000); // 5 sec for testing, should be more on release.
}

RootLink::RootLink(QTcpSocket *socket, QObject *parent):
    AbstractLink(socket, parent),
    m_Contact(NULL),
    m_Timer(NULL)
{
    //mAgent = new PacketAgent();

    connect(m_Socket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(onConnectionError(QAbstractSocket::SocketError)));

    handshake();
}

void RootLink::send(QByteArray &data){
    m_Socket->write(data);
}

QByteArray RootLink::readAll(){
    return m_Socket->readAll();
}

void RootLink::tryConnect(){
    if(m_Socket->state() != QTcpSocket::ConnectedState){
        QString host;
        if(m_HostListIndex > 0){
            m_HostListIndex--;
            host = m_Contact->getHostsList().at(m_HostListIndex);
            m_Socket->abort();
            m_Socket->connectToHost(host,m_Contact->getPort());

            qDebug()<<"try connecting to :" << m_Contact->getName() <<" at : "<<host;
        }else {
            m_HostListIndex = m_Contact->hostsList()->length();
            //error("No host found !  0/"+m_Contact->hostsList()->length());
        }
    }
}

void RootLink::onConnected(){
    m_Timer->stop();

    qDebug()<<m_Contact->getName()<<"connected on : "<<m_Contact->getHostsList().at(m_HostListIndex);
    handshake();
}

void RootLink::handshake(){
    QObject::connect(m_Socket,SIGNAL(readyRead()),
                     this,SIGNAL(newDataAvailable()));
    //QObject::connect(mAgent,SIGNAL(senddata(QByteArray)),this,SLOT(write(QByteArray)));
    //TODO: see who is responsible for agent mgmt
    m_State = Online;
    emit stateChanged(m_State);
}

void RootLink::onConnectionError(QAbstractSocket::SocketError error){
/*    if(error == QAbstractSocket::ConnectionRefusedError);               //The connection was refused by the peer (or timed out).
    if(error == QAbstractSocket::RemoteHostClosedError);                //The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.
    if(error == QAbstractSocket::HostNotFoundError); */       //The host address was not found.

    qDebug()<<"onSocketError";

//    QAbstractSocket::SocketAccessError	3	The socket operation failed because the application lacked the required privileges.
//    QAbstractSocket::SocketResourceError	4	The local system ran out of resources (e.g., too many sockets).
//    QAbstractSocket::SocketTimeoutError	5	The socket operation timed out.
//    QAbstractSocket::DatagramTooLargeError	6	The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).
//    QAbstractSocket::NetworkError	7	An error occurred with the network (e.g., the network cable was accidentally plugged out).
//    QAbstractSocket::AddressInUseError	8	The address specified to QUdpSocket::bind() is already in use and was set to be exclusive.
//    QAbstractSocket::SocketAddressNotAvailableError	9	The address specified to QUdpSocket::bind() does not belong to the host.
//    QAbstractSocket::UnsupportedSocketOperationError	10	The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).
//    QAbstractSocket::ProxyAuthenticationRequiredError	12	The socket is using a proxy, and the proxy requires authentication.
//    QAbstractSocket::SslHandshakeFailedError	13	The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket) (This value was introduced in 4.4.)
//    QAbstractSocket::UnfinishedSocketOperationError	11	Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background). (This value was introduced in 4.4.)
//    QAbstractSocket::ProxyConnectionRefusedError	14	Could not contact the proxy server because the connection to that server was denied (This value was introduced in 4.5.)
//    QAbstractSocket::ProxyConnectionClosedError	15	The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established) (This value was introduced in 4.5.)
//    QAbstractSocket::ProxyConnectionTimeoutError	16	The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase. (This value was introduced in 4.5.)
//    QAbstractSocket::ProxyNotFoundError	17	The proxy address set with setProxy() (or the application proxy) was not found. (This value was introduced in 4.5.)
//    QAbstractSocket::ProxyProtocolError	18	The connection negotiation with the proxy server because the response from the proxy server could not be understood. (This value was introduced in 4.5.)
//    QAbstractSocket::UnknownSocketError	-1	An unidentified error occurred.
}

RootLink::~RootLink(){
    if(m_Timer)
        delete m_Timer;
}
