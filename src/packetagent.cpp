#include "packetagent.h"

PacketAgent::PacketAgent( QPair<QByteArray,QByteArray> key){

    mKey = key.first;
    mIV = key.second;



}
PacketAgent::PacketAgent(){

}

bool PacketAgent::login(){

//    if(m_appList.append(app));

//    emit senddata("login at :" +m_appList.size());



    return 0;
}
bool PacketAgent::logout(){


//    emit senddata("logout"+m_appList.indexOf(app));
//    m_appList.removeAll(app);

    return 0;
}

void PacketAgent::writedata(QByteArray data){

    emit senddata(data);
    qDebug()<<"Sent : "<<data.data();
}

void PacketAgent::incomingdata(QByteArray data){

    mContent = data;

    qDebug()<<"recieved :"+mContent;

    emit readdata(data);


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
