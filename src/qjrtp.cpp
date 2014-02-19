#include "qjrtp.h"

QJrtp::QJrtp(QObject *parent):
    QIODevice(parent)
{

#ifdef WIN32
    WSADATA dat;
    qDebug()<<WSAStartup(MAKEWORD(2,2),&dat);
#endif // WIN32

    mSessionparams.SetOwnTimestampUnit(1.0/8000.0);
    mTransparams.SetPortbase(200);
    int status = mSession.Create(mSessionparams,&mTransparams);
    if (status < 0){
        qWarning()<< QString::fromStdString(RTPGetErrorString(status));
    }
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
    return maxSize;
}

qint64 QJrtp::writeData(const char * data, qint64 maxSize){
    return maxSize;
}

QJrtp::~QJrtp(){
#ifdef WIN32
    WSACleanup();
#endif // WIN32
}
