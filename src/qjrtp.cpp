#include "qjrtp.h"

Qjrtp::Qjrtp(QIODevice* deviceToUse, QObject* parent) :
    QIODevice(parent),
    underlyingDevice(deviceToUse)
{


}

Qjrtp::~Qjrtp(){

}
