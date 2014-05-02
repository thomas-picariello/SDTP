#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QObject>
#include "networkmanager.h"
#include "packetagent.h"


class AbstractApp
{
public:
    AbstractApp(NetworkManager * manager);


public slots :
    void dataToRead(QByteArray *data);


signals :
    void dataToSend(QByteArray);


private :
    ~AbstractApp();

    PacketAgent *m_Root;
    QByteArray *m_Data;



};

#endif // ABSTRACTAPP_H
