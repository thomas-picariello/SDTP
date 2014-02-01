#include "qjrtp.h"

QJrtp::QJrtp(QObject *parent):
    QIODevice(parent)
{
    mSessionparams.SetOwnTimestampUnit(1.0/8000.0);
}

bool QJrtp::open(){
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
