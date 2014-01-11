#ifndef RESPONDER_H
#define RESPONDER_H

#include <QTcpSocket>
#include <QObject>
#include <QHostAddress>
#include "message.h"
#include "messengerwindow.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"

using namespace CryptoPP;

class Responder : public QObject
{
    Q_OBJECT

public:
    Responder(QTcpSocket *socket,  QObject *parent=0 );
    ~Responder();

public slots :
    void error(QAbstractSocket::SocketError);

private :
    QTcpSocket *m_responderSocket;
    MessengerWindow *m_messenger_window;
    QByteArray mAesKey;
    QByteArray mAesIv;
    CFB_Mode<AES>::Encryption mCfbAesEnc;
    CFB_Mode<AES>::Decryption mCfbAesDec;
};

#endif // RESPONDER_H
