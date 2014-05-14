#include "packetagent.h"

PacketAgent::PacketAgent( QPair<QByteArray,QByteArray> key){

    mKey = key.first;
    mIV = key.second;



}
PacketAgent::PacketAgent(){

}
void PacketAgent::logApp(AbstractApp *app){
mAppList.append(app);

connect(app,SIGNAL(dataToSend(QByteArray)),this,SLOT(incomingdata(QByteArray)));
}

void PacketAgent::incomingdata(QByteArray data){

    mContent = data;

    qDebug()<<"recieved :"+mContent;

    mAppList.first()->dataToRead(data);


}
void PacketAgent::encrypt(){

}

void PacketAgent::decrypt(){

}

void PacketAgent::addheader(){

}

void PacketAgent::addHash(){

}

void PacketAgent::makeHash(){

}

void PacketAgent::getHash(){

}

void PacketAgent::parseHeader(){

}

bool PacketAgent::verifyHash(){

    return 0;
}
PacketAgent::~PacketAgent(){

}
