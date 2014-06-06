#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(QList<Contact *> contactList, ContactDB *contactDB, QWidget* parent) :
    AbstractApp(contactList, contactDB, parent),
    ui(new Ui::MessengerApp)
{
    ui->setupUi(this);
    show();

    connect(ui->text_input, SIGNAL(returnPressed()),
            this, SLOT(sendMessage()));
}

void MessengerApp::addContact(){

}

void MessengerApp::updateDisplay(QByteArray msg){
    ui->message_list->setText(msg.data());
}

void MessengerApp::readIncommingData(QByteArray &data){
    updateDisplay(data);
}

void MessengerApp::sendMessage(){
    QByteArray msg = ui->text_input->text().toUtf8();
    int contactID = m_ContactList.first()->getId(); //TODO: improve for multiple contacts
    emit sendData(contactID, TCP, msg);
    updateDisplay(msg); //Loopback for testing
    ui->text_input->clear();
}

MessengerApp::~MessengerApp(){
    //AbstractApp::~AbstractApp();
}
