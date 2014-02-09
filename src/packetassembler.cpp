#include "packetassembler.h"

PacketAssembler::PacketAssembler(Contact *contact,QObject *parent) :
    QObject(parent)
{

    m_contact = contact;
    m_TimeStamp = 0;
    m_PacketCounter = 0;
    m_dateTime = new QDateTime();
    key = m_contact->getKey();






}
QByteArray PacketAssembler::assemble(QByteArray data, quint8 appID){
    m_data = data;
    m_appID = appID;
    m_TimeStamp = m_dateTime->currentMSecsSinceEpoch()%64536;
    m_PacketCounter++;

    data.prepend(appID);
    data.prepend(m_PacketCounter);
    data.prepend(m_TimeStamp&0xff);
    data.prepend((m_TimeStamp>>8)&0xff);

    encrypt();

    return 0;

}
QByteArray PacketAssembler::disassemble(QByteArray data){

    m_data = data;


    return 0;
}
quint8 PacketAssembler::getID(){

    return 0;
}

void PacketAssembler::encrypt(){

}
void PacketAssembler::decrypt(){

}
void PacketAssembler::sync(){

}
void PacketAssembler::refreshKey(){

}
PacketAssembler::~PacketAssembler(){



}

