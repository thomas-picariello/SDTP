#include "qjrtp.h"

QJrtp::QJrtp(/*Contact *contact,*/ QObject *parent):
    QIODevice(parent)/*,
    mContact(contact)*/
{
    mSessionparams.SetOwnTimestampUnit(1.0/8000.0);
    mTransparams.SetPortbase(8000);
    //Debug loopback
    mSessionparams.SetAcceptOwnPackets(true);
}

bool QJrtp::open(){
    /*int status = mSession.Create(mSessionparams,&mTransparams);
    if (status < 0){
        qWarning()<< QString::fromStdString(RTPGetErrorString(status));
        return false;
    }
    quint8 ip[] = {127,0,0,1};
    RTPIPv4Address addr(ip, 8000);
    status = mSession.AddDestination(addr);
    if (status < 0){
        qWarning()<< QString::fromStdString(RTPGetErrorString(status));
        return false;
    }
    mSession.SetDefaultPayloadType(96); //first dynamic payload type defined by RFC 3551
    mSession.SetDefaultMark(false);
    mSession.SetDefaultTimestampIncrement(160);*/

    setOpenMode(ReadWrite);
    return true;
}

void QJrtp::close(){
    /*RTPTime delay(10.0);
    mSession.BYEDestroy(delay,"Time's up",9);*/
    setOpenMode(NotOpen);
}

bool QJrtp::isSequential() const{
    return true;
}

qint64 QJrtp::readData(char * data, qint64 maxSize){
    /*mSession.BeginDataAccess();
    if (mSession.GotoFirstSource()){ //assuming we have only one source
        RTPPacket *packet;
        while ((packet = mSession.GetNextPacket()) != 0){
            qDebug() << "Got packet with extended sequence number "
            << packet->GetExtendedSequenceNumber()
            << " from SSRC " << packet->GetSSRC();
            mSession.DeletePacket(packet);
        }
    }
    mSession.EndDataAccess();*/
    return maxSize;
}

qint64 QJrtp::writeData(const char * data, qint64 maxSize){
    /*uint8_t silencebuffer[160];

    for (int i = 0 ; i < 160 ; i++)
           silencebuffer[i] = 128;

    int status = mSession.SendPacket(silencebuffer,160);
    if (status < 0){
        qWarning()<< QString::fromStdString(RTPGetErrorString(status));
        return false;
    }*/
    return maxSize;
}


QJrtp::~QJrtp(){

}
