#ifndef HANDSHAKERESPONDER_H
#define HANDSHAKERESPONDER_H

#include "abstracthandshake.h"
#include "contactdb.h"

class HandshakeResponder : public AbstractHandshake
{
    Q_OBJECT
public:
    explicit HandshakeResponder(QTcpSocket* socket,
                                ContactDB* contactDB,
                                IpFilter* ipFilter,
                                RsaKeyring* keyring,
                                QObject* parent = 0);

    virtual Mode getMode() const{ return ResponderMode; }

private slots:
    void responderParseStarterHello();
    void responderParseHalfKeyAndResponderIntegrity();
    void responderParseHandshakeFinished();

private:
    ContactDB* m_contactDB;

    void responderRespondHello();
    void responderSendStarterIntegrity();

    Q_DISABLE_COPY(HandshakeResponder)
};

#endif // HANDSHAKERESPONDER_H
