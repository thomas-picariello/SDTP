#ifndef MESSENGERAPP_H
#define MESSENGERAPP_H

#include <contact.h>
#include <QList>
#include <QtWebKitWidgets/QWebView>
#include "abstractapp.h"
#include "networkmanager.h"

namespace Ui{
class MessengerApp;

}


class MessengerApp : public AbstractApp
{

    Q_OBJECT

public:
    explicit MessengerApp(NetworkManager *manager, QWidget *parent = 0);
    void addContact(NetworkManager* manager);

private:
    Ui::MessengerApp *ui;

};

#endif // MESSENGERAPP_H
