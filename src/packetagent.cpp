#include "packetagent.h"

PacketAgent::PacketAgent( QPair<QByteArray,QByteArray> key){

    mKey = key.first;
    mIV = key.second;




    qDebug()<<"PacketAgent::PacketAgent( QPair) Done";


}
PacketAgent::PacketAgent(){

}

bool PacketAgent::login(){

    senddata("qwertzu");

    return 0;
}
bool PacketAgent::logout(QObject* parent){

    return 0;
}
void PacketAgent::newdata(QByteArray data){

    mContent = data;

    qDebug()<<"recieved :"+mContent;

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
