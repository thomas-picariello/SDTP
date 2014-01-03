#ifndef MESSERGERWINDOW_H
#define MESSERGERWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QByteArray>
#include <QList>
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"
#include "message.h"

namespace Ui {
class MessengerWindow;
}

class MessengerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MessengerWindow(QWidget *parent = 0);
    ~MessengerWindow();

signals:
    void sendMessage(QByteArray data);

public slots:
    void displayMessage(Message msg);
    void onSend();

private:
    Ui::MessengerWindow *ui;
    QList<Message> mMsgList;
    void updateChat();
};

#endif // MESSERGERWINDOW_H
