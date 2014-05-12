#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>

#include "networkmanager.h"
#include "packetagent.h"



class AbstractApp : public QWidget
{

    Q_OBJECT

public:
    AbstractApp(NetworkManager * manager, QWidget *parent = 0);
    ~AbstractApp();

public slots :
    void dataToRead(QByteArray *data);


signals :
    void dataToSend(QByteArray);


protected :


    QByteArray *m_Data;
    QList<PacketAgent*> *m_AgentList;



};

#endif // ABSTRACTAPP_H
