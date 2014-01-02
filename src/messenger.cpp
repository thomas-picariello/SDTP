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
    connect(getmsg,SIGNAL(returnPressed()),this,SLOT(onSend()));

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
    //qDebug()<<"about to emit";
    QByteArray msg = getmsg->text().toUtf8();

    /****AES encryption in CFB mode***/
    QByteArray key(16, 0x0); //16 byte = 128 bits, filled with 0x0.
    qDebug()<<"AES key:"<<key.toHex();

    QByteArray iv(CryptoPP::AES::BLOCKSIZE, 0x0); //filled with 0x0.
    qDebug()<<"AES iv:"<<iv.toHex();

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEnc;
    cfbEnc.SetKeyWithIV((byte*)key.data(), key.size(), (byte*)iv.data());
    cfbEnc.ProcessData((byte*)msg.data(), (byte*)msg.data(), msg.size());

    emit sendMessage(msg);
    //displayMessage("Sent : "+getmsg->text());
    getmsg->clear();
    //qDebug()<<"emited";
}
Messenger::~Messenger()
{
    delete messenger;
    delete messageDisplay;
    delete getmsg;
    delete sendmsg;
    delete msglist;
    delete sendlayout;
    delete msglayout;
}
