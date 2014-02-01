#ifndef QJRTP_H
#define QJRTP_H

#include <QIODevice>
#include <QBuffer>
#include <jrtplib/rtpsession.h>
#include <jrtplib/rtpsessionparams.h>
#include <jrtplib/rtpudpv4transmitter.h>
#include <jrtplib/rtpipv4address.h>
#include <jrtplib/rtptimeutilities.h>
#include <jrtplib/rtppacket.h>
//#include <stdlib.h>
//#include <iostream>

using namespace jrtplib;

class QJrtp : public QIODevice
{
    Q_OBJECT

public:
    explicit QJrtp(QObject* parent = 0);
    ~QJrtp();

    bool open();
    void close();
    bool isSequential() const;

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private :
    RTPSession mSession;
    RTPSessionParams mSessionparams;
    RTPUDPv4TransmissionParams mV4Transparams;
};

#endif // QJRTP_H
