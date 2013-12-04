#ifndef MESSENGER_H
#define MESSENGER_H


#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>



class Messenger
{
public:
    Messenger();
    ~Messenger();

public slots:
    void displaymsg(QString);
    void sendnewmsg();

private :
    QWidget *messenger;
    QLabel *messageDisplay;
    QLineEdit *getmsg;
    QPushButton *sendmsg;
    QStringList *msglist;
    QHBoxLayout *sendlayout;
    QVBoxLayout *msglayout;


};

#endif // MESSENGER_H
