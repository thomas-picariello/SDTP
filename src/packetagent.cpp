#include "packetagent.h"

PacketAgent::PacketAgent( QPair<QByteArray,QByteArray> key){

    mKey = key.first;
    mIV = key.second;


}

bool PacketAgent::login(QObject* parent){

    return 0;
}
bool PacketAgent::logout(QObject* parent){

    return 0;
}
void PacketAgent::send(QByteArray data, QObject* parent){

}
QByteArray PacketAgent::read(QObject* parent){

    mContent = 0;

    return mContent;
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
