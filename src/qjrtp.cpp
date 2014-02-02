#include "qjrtp.h"

QJrtp::QJrtp(Contact *contact, QObject *parent):
    QIODevice(parent),
    mContact(contact)
{
    mSessionparams.SetOwnTimestampUnit(1.0/8000.0);
    mTransparams.SetPortbase(8000);
}

bool QJrtp::open(){
    int status = mSession.Create(mSessionparams,&mTransparams);
    if (status < 0){
        qWarning()<< QString::fromStdString(RTPGetErrorString(status));
        return false;
    }
    QHostAddress addr = mContact->getIpAddress();
    //TODO: define addr in rtp
    setOpenMode(ReadWrite);
    return true;
}

void QJrtp::close(){
    setOpenMode(NotOpen);
}

bool QJrtp::isSequential() const{
    return true;
}

qint64 QJrtp::readData(char * data, qint64 maxSize){
    return 0;
}

qint64 QJrtp::writeData(const char * data, qint64 maxSize){
    return 0;
}


QJrtp::~QJrtp(){

}
