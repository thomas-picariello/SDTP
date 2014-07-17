#include "voiceapp.h"
#include"ui_voiceapp.h"

VoiceApp::VoiceApp(Contact* contact, QWidget* parent) :
    AbstractApp(contact, parent), ui(new Ui::VoiceApp())
{
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    if(contact->getStatus() != Contact::Offline)
        m_state = Ready;
    else
        m_state = Disconnected;
    onStateChange();

    ui->mic_monitor_pb->setInterpolationTime(MONITORS_UPDATE_INTERVAL);
    ui->output_monitor_pb->setInterpolationTime(MONITORS_UPDATE_INTERVAL);
    connect(&m_monitorUpdateTimer, &QTimer::timeout, this, &VoiceApp::updateMonitorsValue);
    m_monitorUpdateTimer.start(MONITORS_UPDATE_INTERVAL);
}


void VoiceApp::readIncommingData(QByteArray& data){

}

void VoiceApp::endCall(){
    m_state = Ready;
    onStateChange();
}

void VoiceApp::onContactStatusChange(){
    Contact* contact = dynamic_cast<Contact*>(sender());
    if(contact){
        onStateChange();
    }
}

void VoiceApp::startCall(){
    m_state = Calling;
    onStateChange();
}

void VoiceApp::updateMonitorsValue(){
    ui->mic_monitor_pb->setValue(qrand() % (ui->mic_vol_sl->value()+1));
    ui->output_monitor_pb->setValue(qrand() % (ui->output_vol_sl->value()+1));
}

void VoiceApp::onStateChange(){
    QColor buttonColor;
    disconnect(ui->call_bt, &QPushButton::clicked, this, 0);
    switch(m_state){
    case Disconnected:
        buttonColor.setNamedColor("#bbb");
        ui->call_bt->setIcon(QIcon(":/icons/call"));
        break;
    case Ready:
        buttonColor.setNamedColor("#0b0");
        ui->call_bt->setIcon(QIcon(":/icons/call"));
        connect(ui->call_bt, &QPushButton::clicked, this, &VoiceApp::startCall);
        break;
    case Calling:
        buttonColor.setNamedColor("#b00");
        ui->call_bt->setIcon(QIcon(":/icons/hangup"));
        connect(ui->call_bt, &QPushButton::clicked, this, &VoiceApp::endCall);
        break;
    }
    ui->call_bt->setStyleSheet("QPushButton{background:"         + buttonColor.name() +";}"
                               "QPushButton::hover{background:"  + buttonColor.lighter(110).name() +";}"
                               "QPushButton::pressed{background:"+ buttonColor.darker(110).name() +";}");

    //update contact names color
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
        label->setStyleSheet("QLabel{color:"+ color +";}");
        QFont font = label->font();
        font.setPointSize(12);
        label->setFont(font);
        label->setAlignment(Qt::AlignHCenter);
        ui->names_layout->addWidget(label);
    }
    update();
}

VoiceApp::~VoiceApp(){
    delete ui;
}
