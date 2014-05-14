#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>
#include "packetagent.h"



class AbstractApp : public QWidget
{

    Q_OBJECT

public:
    AbstractApp(PacketAgent *agent, QWidget *parent = 0);
    ~AbstractApp();

public slots :
    void dataToRead(QByteArray data);


signals :
    void dataToSend(QByteArray);

private :



};

#endif // ABSTRACTAPP_H
