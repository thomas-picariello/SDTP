#include "abstractlink.h"

AbstractLink::AbstractLink(Contact contact)
{


}
AbstractLink::AbstractLink(QTcpSocket socket){


}
const AbstractLink::STATE AbstractLink::state(){

return AbstractLink::STATE::ERROR;
}
void AbstractLink::write(QByteArray data){

}
QByteArray AbstractLink::read(){

    QByteArray mContent;
return mContent;
}
void AbstractLink::connect(){

}
void AbstractLink::Handshake(){

}
