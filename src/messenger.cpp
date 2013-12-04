#include "messenger.h"

Messenger::Messenger()
{
    messenger = new QWidget;
    messageDisplay = new QLabel;
    getmsg = new QLineEdit;
    sendmsg = new QPushButton;
    msglist = new QStringList;
    sendlayout = new QHBoxLayout;
    msglayout = new QVBoxLayout;


    getmsg->setPlaceholderText("Write down your deepest thougts and send them to me...");
    sendmsg->setText("Send");


    sendlayout->addWidget(getmsg);
    sendlayout->addWidget(sendmsg);

    msglayout->addWidget(messageDisplay);
    msglayout->addLayout(sendlayout);

    messenger->setLayout(msglayout);

    for(int i=0;i<10;i++)msglist->append("|");


    ////////////Test area
    ///

    msglist->removeFirst();
    msglist->append("|>>message of me or someone else :D ");
    msglist->removeFirst();
    msglist->append("|<<message of someone else or me ;) ");


    ///
    ///////////

    messageDisplay->setText(msglist->join("\n"));


    messenger->show();




}
void Messenger::displaymsg(QString msg)
{
    messenger->show();

}
void Messenger::sendnewmsg()
{

}
Messenger::~Messenger()
{

}
