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
    explicit MessengerWindow(QWidget *parent = 0);
    void changeButtonState(bool state);
    ~MessengerWindow();

signals:
    void sendMessage(QByteArray data);
    void callContact();

public slots:
    void displayMessage(Message msg);
    void onSend();

private:
    Ui::MessengerWindow *ui;
    QByteArray mKey;
    QByteArray mIv;
    QList<Message> mMsgList;
    void updateChat();
};

#endif // MESSERGERWINDOW_H
