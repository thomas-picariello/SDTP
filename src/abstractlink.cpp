#include "abstractlink.h"

AbstractLink::AbstractLink(Contact *contact)
{
    mState = OFFLINE;
    mSocket = new QTcpSocket;
    m_Contact = contact;
    mAgent = new PacketAgent(QPair<QByteArray,QByteArray>(m_Contact->getKey(),"000"));
    HostListLength = m_Contact->hostsList()->length();
    connect(mSocket,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(mSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketError(QAbstractSocket::SocketError)));

    tryConnect();


}
AbstractLink::AbstractLink(QTcpSocket *socket){

    mState = OFFLINE;
    mSocket = socket;
    mAgent = new PacketAgent();
    handshake();
    connect(mSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketError(QAbstractSocket::SocketError)));


}
AbstractLink::State AbstractLink::state(){
return mState;
}
void AbstractLink::write(QByteArray data){
    mSocket->write(data);
}
void AbstractLink::read(){

    mAgent->incomingdata(mSocket->readAll());


}
void AbstractLink::tryConnect(){

    if(HostListLength>0){
        HostListLength--;
        mSocket->abort();
        mSocket->connectToHost(m_Contact->getHostsList().at(HostListLength),m_Contact->getPort());
    }
    else error("No host found !  0/"+m_Contact->hostsList()->length());

}

void AbstractLink::onConnected(){


    handshake();

}


void AbstractLink::handshake(){





    QObject::connect(mSocket,SIGNAL(readyRead()),this,SLOT(read()));
    QObject::connect(mAgent,SIGNAL(senddata(QByteArray)),this,SLOT(write(QByteArray)));

    mState = ONLINE;
    emit connected();


}
PacketAgent *AbstractLink::getagent(){
    return mAgent;
}

void AbstractLink::onSocketError(QAbstractSocket::SocketError error){

    if(error == QAbstractSocket::ConnectionRefusedError);               //The connection was refused by the peer (or timed out).
    if(error == QAbstractSocket::RemoteHostClosedError);                //The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.
    if(error == QAbstractSocket::HostNotFoundError)tryConnect();        //The host address was not found.








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

AbstractLink::~AbstractLink(){

}
