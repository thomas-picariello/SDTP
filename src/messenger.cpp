#include "messenger.h"

Messenger::Messenger(QObject *parent): QObject(parent)
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

    for(int i=0;i<10;i++)msglist->append(" ");

    messageDisplay->setText(msglist->join("\n"));

    connect(sendmsg, SIGNAL(clicked()),
            this, SLOT(onSend()));

}

void Messenger::displayMessage(QString msg)
{
    messenger->show();
    msglist->removeFirst();
    msglist->append(msg);
    messageDisplay->setText(msglist->join("\n| "));
}
void Messenger::onSend()
{
    qDebug()<<"about to emit";
    emit sendMessage(getmsg->text().toUtf8());
    displayMessage("Sent : "+getmsg->text());
    getmsg->clear();
    qDebug()<<"emited";

}
Messenger::~Messenger()
{

}
