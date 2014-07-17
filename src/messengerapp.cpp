#include "messengerapp.h"
#include "ui_messengerapp.h"


MessengerApp::MessengerApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::MessengerApp)
{
    ui->setupUi(this);

    m_MsgList.append("<div align='center' style='color:grey; margin: 10px;'>Welcome to the Messenger App</div>");
    updateDisplay();

    connect(ui->text_input, SIGNAL(returnPressed()),
            this, SLOT(sendMessage()));
}

void MessengerApp::addContact(){

}

void MessengerApp::updateDisplay(){

    const QString header = "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\
            <html><body style=' font-family:'MS Shell Dlg 2'; font-size:9pt; font-weight:400; font-style:normal;'><p>";
    const QString footer = "</p></body></html>";

    QString html = header;

    for(int i=0;i<=m_MsgList.length();i++)html.append(m_MsgList.value(i));

    html.append(footer);

    ui->message_list->setHtml(html);

    ui->message_list->verticalScrollBar()->setValue(ui->message_list->verticalScrollBar()->maximum());
}

void MessengerApp::readIncommingData(QByteArray &data){
    m_MsgList.append("<div align='left' style='color:green; margin: 10px;'>"+data+"</div>");

    updateDisplay();
}

void MessengerApp::sendMessage(){
    QByteArray msg = ui->text_input->text().toUtf8();
    emit sendData(TCP, msg);

    m_MsgList.append("<div align='right' style='color:blue; margin: 10px;'>"+msg+"</div>");

    updateDisplay();
    ui->text_input->clear();
}

MessengerApp::~MessengerApp(){
    delete ui;
}
