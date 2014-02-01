#include "qjrtp.h"

QJrtp::QJrtp(QIODevice* deviceToUse, QObject* parent) :
    QIODevice(parent),
    underlyingDevice(deviceToUse)
{


}

QJrtp::~QJrtp(){

}
