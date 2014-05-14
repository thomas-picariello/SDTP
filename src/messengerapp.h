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
    explicit MessengerApp( QWidget *parent = 0);
    void addContact();
    void updateDisplay(QByteArray data);

public slots :
    virtual void dataToSend(QByteArray data);



private slots:
    void on_mGetText_returnPressed();

private:
    Ui::MessengerApp *ui;

};

#endif // MESSENGERAPP_H
