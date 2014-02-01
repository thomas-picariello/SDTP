#ifndef QJRTP_H
#define QJRTP_H

#include <QIODevice>

class QJrtp : public QIODevice
{
    Q_OBJECT

public:
    QJrtp(QIODevice* deviceToUse, QObject* parent = 0);
    ~QJrtp();

private :
    QIODevice* underlyingDevice;
};

#endif // QJRTP_H
