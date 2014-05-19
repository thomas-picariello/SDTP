#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(QWidget* parent) :
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

void MessengerApp::dataToRead(QByteArray data){
    updateDisplay(data);
}

void MessengerApp::sendMessage(){
    QByteArray msg = ui->text_input->text().toUtf8();
    emit dataToSend(msg);
    updateDisplay(msg); //Loopback for testing
    ui->text_input->clear();
}
