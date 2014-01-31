#ifndef QJRTP_H
#define QJRTP_H

#include <QIODevice>

class Qjrtp : public QIODevice
{
    Q_OBJECT

public:
    Qjrtp(QIODevice* deviceToUse, QObject* parent = 0);
    ~Qjrtp();

private :
    QIODevice* underlyingDevice;
};

#endif // QJRTP_H
