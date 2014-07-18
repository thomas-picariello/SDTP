#include "voiceapp.h"
#include"ui_voiceapp.h"

VoiceApp::VoiceApp(Contact* contact, QWidget* parent) :
    AbstractApp(contact, parent), m_ui(new Ui::VoiceApp())
{
    setWindowFlags(Qt::WindowCloseButtonHint);
    m_ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    if(contact->getStatus() != Contact::Offline)
        m_state = Ready;
    else
        m_state = Disconnected;
    onStateChange();

    m_ui->mic_monitor_pb->setInterpolationTime(MONITORS_UPDATE_INTERVAL);
    m_ui->output_monitor_pb->setInterpolationTime(MONITORS_UPDATE_INTERVAL);
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
    m_ui->mic_monitor_pb->setValue(qrand() % (m_ui->mic_vol_sl->value()+1));
    m_ui->output_monitor_pb->setValue(qrand() % (m_ui->output_vol_sl->value()+1));
}

void VoiceApp::onStateChange(){
    QColor buttonColor;
    disconnect(m_ui->call_bt, &QPushButton::clicked, this, 0);
    switch(m_state){
    case Disconnected:
        buttonColor.setNamedColor("#bbb");
        m_ui->call_bt->setIcon(QIcon(":/icons/call"));
        break;
    case Ready:
        buttonColor.setNamedColor("#0b0");
        m_ui->call_bt->setIcon(QIcon(":/icons/call"));
        connect(m_ui->call_bt, &QPushButton::clicked, this, &VoiceApp::startCall);
        break;
    case Calling:
        buttonColor.setNamedColor("#b00");
        m_ui->call_bt->setIcon(QIcon(":/icons/hangup"));
        connect(m_ui->call_bt, &QPushButton::clicked, this, &VoiceApp::endCall);
        break;
    }
    m_ui->call_bt->setStyleSheet("QPushButton{background:"         + buttonColor.name() +";}"
                               "QPushButton::hover{background:"  + buttonColor.lighter(110).name() +";}"
                               "QPushButton::pressed{background:"+ buttonColor.darker(110).name() +";}");

    //update contact names color
    QLayoutItem *item;
    while((item = m_ui->names_layout->takeAt(0)) != 0)
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
        m_ui->names_layout->addWidget(label);
    }
    update();
}

VoiceApp::~VoiceApp(){
    delete m_ui;
}
