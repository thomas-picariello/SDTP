#include "packet.h"

Packet::Packet(QByteArray content,APPID appid){

    mContent = content;
    mAppID = appid;

    addheader();
    makeHash();
    encrypt();
    addHash();

}
Packet::Packet(QByteArray data){
    mData = data;

    getHash();
    decrypt();
    if(verifyHash())parseHeader();
    else{
        mAppID = Packet::ERROR;
        mContent = 0;
    }

}
const QByteArray Packet::content(){
    return mContent;
}
const Packet::APPID Packet::appid(){
    return mAppID;
}
const QByteArray Packet::data(){
    return mData;
}
void Packet::encrypt(){

}
void Packet::decrypt(){

}
void Packet::addheader(){

}
void Packet::addHash(){

}
void Packet::makeHash(){

}
void Packet::getHash(){

}
void Packet::parseHeader(){

}
bool Packet::verifyHash(){


    return true; //Hash good or not
}
