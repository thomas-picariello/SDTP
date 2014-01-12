#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include <QSettings>
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"

using namespace CryptoPP;

class Handshake : public QObject
{

    Q_OBJECT

public:
    Handshake();
};

#endif // HANDSHAKE_H
