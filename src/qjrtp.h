#ifndef QJRTP_H
#define QJRTP_H

#include <QDebug>
#include <QIODevice>
#include <QBuffer>
#include <QHostAddress>
#include <jrtplib/rtpsession.h>
#include <jrtplib/rtpsessionparams.h>
#include <jrtplib/rtpudpv4transmitter.h>
#include <jrtplib/rtpipv4address.h>
#include <jrtplib/rtptimeutilities.h>
#include <jrtplib/rtppacket.h>
#include "contact.h"

using namespace jrtplib;

class QJrtp : public QIODevice
{
    Q_OBJECT

public:
    explicit QJrtp(Contact *contact, QObject* parent = 0);
    ~QJrtp();

    bool open();
    void close();
    bool isSequential() const;

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private :
    Contact *mContact;
    RTPSession mSession;
    RTPSessionParams mSessionparams;
    RTPUDPv4TransmissionParams mTransparams;
};

#endif // QJRTP_H
