#ifndef MESSENGERAPP_H
#define MESSENGERAPP_H

#include <contact.h>
#include <QList>
#include "abstractapp.h"

namespace Ui{
class MessengerApp;

}

class MessengerApp : public AbstractApp
{
    Q_OBJECT

public:
    static const uint APPID = 1;

    explicit MessengerApp(QList<Contact*> contactList, ContactDB *contactDB, QWidget *parent = 0);
    virtual ~MessengerApp();

    void addContact();
    void updateDisplay(QByteArray msg);

public slots :
    virtual void readIncommingData(QByteArray &data);

private slots:
    void sendMessage();

private:
    Ui::MessengerApp *ui;

};

#endif // MESSENGERAPP_H
