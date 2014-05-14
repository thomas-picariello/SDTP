#ifndef MESSENGERAPP_H
#define MESSENGERAPP_H

#include <contact.h>
#include <QList>
#include "abstractapp.h"
#include "networkmanager.h"

namespace Ui{
class MessengerApp;

}


class MessengerApp : public AbstractApp
{

    Q_OBJECT

public:
    explicit MessengerApp(PacketAgent *agent, QWidget *parent = 0);
    void addContact(PacketAgent* agent);



private:
    Ui::MessengerApp *ui;

};

#endif // MESSENGERAPP_H
