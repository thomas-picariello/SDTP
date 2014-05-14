#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(QWidget* parent) :
    AbstractApp(parent),
    ui(new Ui::MessengerApp)
{


    ui->setupUi(this);
    show();



    emit dataToSend("hahahaha");
}

void MessengerApp::addContact()
{

    //m_AgentList.append(manager->getRootAgent());

}
void MessengerApp::updateDisplay(QByteArray data){

    ui->textEdit->setText(data.data());
}
void MessengerApp::dataToSend(QByteArray data){

    AbstractApp::dataToSend(data);

    updateDisplay(data);
}

void MessengerApp::on_mGetText_returnPressed()
{
    emit dataToSend((QByteArray)ui->mGetText->text().toUtf8());
}
