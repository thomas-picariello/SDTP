#ifndef MESSERGERWINDOW_H
#define MESSERGERWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QByteArray>
#include <QList>
#include <QScrollBar>

#include "message.h"

namespace Ui {
class MessengerWindow;
}

class MessengerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MessengerWindow(qint8,QWidget *parent = 0);
    void changeButtonState(bool state);
    ~MessengerWindow();

signals:
    void sendMessage(QByteArray data,qint8 appID);
    void callContact();

public slots:
    void displayMessage(Message msg);
    void onSend();

private:
    Ui::MessengerWindow *ui;
    qint8 appID;
    QByteArray mKey;
    QByteArray mIv;
    QList<Message> mMsgList;
    void updateChat();
};

#endif // MESSERGERWINDOW_H
