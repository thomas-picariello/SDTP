#ifndef MESSENGERAPP_H
#define MESSENGERAPP_H

#include <contact.h>
#include <QList>
#include <QScrollBar>
#include "typesenums.h"
#include "abstractapp.h"

namespace Ui{
class MessengerApp;

}

class MessengerApp : public AbstractApp
{
    Q_OBJECT

public:
    static const AppType APPTYPE = Messenger;

    explicit MessengerApp(Contact *contact, QWidget *parent = 0);
    virtual ~MessengerApp();

    void addContact();
    void updateDisplay();

public slots :
    virtual void readIncommingData(const QByteArray& data);

private slots:
    void sendMessage();

private:
    Ui::MessengerApp *ui;
    QStringList m_MsgList;

};

#endif // MESSENGERAPP_H
