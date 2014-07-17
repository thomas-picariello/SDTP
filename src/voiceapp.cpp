#include "voiceapp.h"
#include"ui_voiceapp.h"

VoiceApp::VoiceApp(Contact* contact, QWidget* parent) :
    AbstractApp(contact, parent), ui(new Ui::VoiceApp())
{
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    updateContactNames();
}


void VoiceApp::readIncommingData(QByteArray& data){

}

void VoiceApp::updateContactNames(){
    //clear layout
    QLayoutItem *item;
    while((item = ui->names_layout->takeAt(0)) != 0)
        delete item->widget();

    foreach(Contact* contact, m_contactList){
        QLabel* label = new QLabel(contact->getName(), this);
        QString color;
        if(contact->getStatus() == Contact::Offline)
            color = "black";
        else
            color = "green";
        label->setStyleSheet("QLabel{"
                                "color:"+ color +";"
                                "font: 14px;"
                             "}");
        label->setAlignment(Qt::AlignHCenter);
        ui->names_layout->addWidget(label);
    }
}

VoiceApp::~VoiceApp(){
    delete ui;
}
