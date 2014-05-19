#include "packetagent.h"

PacketAgent::PacketAgent(){

}

PacketAgent::PacketAgent( QPair<QByteArray,QByteArray> key){

    mKey = key.first;
    mIV = key.second;
}

uint PacketAgent::logApp(AbstractApp *app, AppTypeID appTypeId){
    AbstractApp::AppUID appUID(appTypeId);
    mAppMap.insert(appUID, app);
    return appUID.appInstanceID;
}

void PacketAgent::routeToApp(AbstractApp::AppUID appUID, QByteArray data){

    // parse header etc...
    mAppMap.value(mRouteMap.value(appUID))->dataToRead(data);


}
void PacketAgent::routeToLink(AbstractApp::AppUID appUID, QByteArray data){


    //add header etc...
    emit senddata(data);

}

void PacketAgent::incomingdata(QByteArray data){

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
PacketAgent::~PacketAgent(){

}
