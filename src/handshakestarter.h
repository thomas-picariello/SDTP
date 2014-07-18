#ifndef HANDSHAKESTARTER_H
#define HANDSHAKESTARTER_H

#include "abstracthandshake.h"

class HandshakeStarter : public AbstractHandshake
{
    Q_OBJECT
public:
    explicit HandshakeStarter(QTcpSocket* socket,
                              Contact* contact,
                              IpFilter* ipFilter,
                              RsaKeyring* keyring,
                              QObject* parent = 0);

    virtual Mode getMode() const{ return StarterMode; }

private slots:
    void starterParseResponderHello();
    void starterParseStarterIntegrity();
private:

    void starterSayHello();
    void starterSendHalfKeyAndResponderIntegrity();
    void starterSendHandshakeFinished();


    Q_DISABLE_COPY(HandshakeStarter)
};

#endif // HANDSHAKESTARTER_H
