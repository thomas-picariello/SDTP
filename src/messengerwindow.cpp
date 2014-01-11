#include "messengerwindow.h"
#include "ui_messengerwindow.h"

MessengerWindow::MessengerWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MessengerWindow){
    ui->setupUi(this);


    connect(ui->send, SIGNAL(clicked()),
            this, SLOT(onSend()));
    connect(ui->call, SIGNAL(clicked()),
            this, SLOT(onCall()));
    connect(ui->input,SIGNAL(returnPressed()),
            this,SLOT(onSend()));
}
void MessengerWindow::changeButtonState(bool state)
{

    if(state == true)ui->call->setText("Call");
    else if (state == false)ui->call->setText("Hang up");
    else ui->call->setText("ERROR");

}

void MessengerWindow::displayMessage(Message msg){
    mMsgList.append(msg);
    this->updateChat();
    this->show();
}

void MessengerWindow::onSend(){
    QByteArray msg = ui->input->text().toUtf8();

    emit sendMessage(msg);
    ui->input->clear();
}
void MessengerWindow::onCall(){

    qDebug()<<"callcontact : emit";
    emit callContact();


}

void MessengerWindow::updateChat(){
    const QString header = "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\
            <html><body style=' font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;'><p>";
    const QString footer = "</p></body></html>";

    QString html = header;

    foreach(Message msg, mMsgList){
        QString align, color;

        switch(msg.type()){
        case Message::SENT:
            align = "right";
            color = "blue";
            break;
        case Message::RECIEVED:
            align = "left";
            color = "green";
            break;
        case Message::ERR:
            align = "center";
            color = "red";
            break;
        default:
            align = "center";
            color = "gray";
            break;
        }
        html.append("<div align='"+align+"' style='color:"+color+"; margin: 10px;'>"+msg.text()+"</div>");
    }
    html.append(footer);
    ui->chat->setHtml(html);
    ui->chat->verticalScrollBar()->setValue(ui->chat->verticalScrollBar()->maximumHeight());
}

MessengerWindow::~MessengerWindow(){
    delete ui;
}
