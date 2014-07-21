#include "voiceapp.h"
#include"ui_voiceapp.h"

VoiceApp::VoiceApp(Contact* contact, QWidget* parent) :
    AbstractApp(contact, parent),
    m_ui(new Ui::VoiceApp())
{
    setWindowFlags(Qt::WindowCloseButtonHint);
    m_ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    if(contact->getStatus() != Contact::Offline)
        m_state = Ready;
    else
        m_state = Disconnected;
    updateUiToState();

    m_codec.setBitrate(25000);
    connect(&m_codec, &OpusVoiceCodec::readyRead, this, &VoiceApp::onCodecReadyRead);

    m_ui->mic_monitor_pb->setInterpolationTime((int)m_codec.getOpusFrameSize());
    m_ui->output_monitor_pb->setInterpolationTime((int)m_codec.getOpusFrameSize());
    connect(&m_codec, &OpusVoiceCodec::newInputProbe, this, &VoiceApp::onNewInputProbe);
    connect(&m_codec, &OpusVoiceCodec::newOutputProbe, this, &VoiceApp::onNewOutputProbe);

    connect(m_ui->call_bt, &QPushButton::clicked, this, &VoiceApp::onCallButtonClick);
    connect(m_ui->output_vol_sl, &QSlider::valueChanged, this, &VoiceApp::setOutputVolume);
    connect(m_ui->mic_mute_bt, &QToolButton::clicked, this, &VoiceApp::setInputMute);
    connect(m_ui->output_mute_bt, &QToolButton::clicked, this, &VoiceApp::setOutputMute);
}


void VoiceApp::readIncommingData(const QByteArray& data){
    QByteArray myData = data;
    Packet packet;
    QDataStream(&myData, QIODevice::ReadOnly) >> packet;
    switch(packet.type){
    case Packet::Control:
        switch((Controls)packet.payload.at(0)){
        case EndCall:
            m_state = Ready;
            endCall();
            break;
        case StartCall:
            m_state = Calling;
            startCall();
            break;
        }
        break;
    case Packet::Data:
        m_codec.write(packet.payload);
        break;
    }
}

void VoiceApp::endCall(){
    m_codec.stop();
    m_ui->mic_monitor_pb->setValue(0);
    m_ui->output_monitor_pb->setValue(0);
    updateUiToState();
}

void VoiceApp::onContactStatusChange(){
    Contact* contact = dynamic_cast<Contact*>(sender());
    if(contact){
        updateUiToState();
    }
}

void VoiceApp::onCodecReadyRead(){
    sendPacket(Packet::Data, m_codec.readAll());
}

void VoiceApp::onCallButtonClick(){
    QByteArray control;
    if(m_state == Ready){
        m_state = Calling;
        startCall();
        control.append((char)StartCall);
        sendPacket(Packet::Control, control);
    }else if(m_state == Calling){
        m_state = Ready;
        endCall();
        control.append((char)EndCall);
        sendPacket(Packet::Control, control);
    }
}

void VoiceApp::startCall(){
    m_codec.start();
    updateUiToState();
}

void VoiceApp::onNewInputProbe(qint16 probe){
    m_ui->mic_monitor_pb->setValue((probe/32767.0)*100);
}

void VoiceApp::onNewOutputProbe(qint16 probe){
    m_ui->output_monitor_pb->setValue((probe/32767.0)*100);
}

void VoiceApp::setOutputVolume(int value){
    m_codec.setOutputVolume(value/100.0);
}

void VoiceApp::setInputMute(bool mute){
    m_codec.setInputMute(mute);
    m_ui->mic_monitor_pb->setValue(0);
}

void VoiceApp::setOutputMute(bool mute){
    m_codec.setOutputMute(mute);
    m_ui->output_monitor_pb->setValue(0);
}

void VoiceApp::closeEvent(QCloseEvent* event){
    event->accept();
    endCall();
}

void VoiceApp::sendPacket(Packet::Type type, const QByteArray& payload){
    QByteArray packetBytes;
    QDataStream(&packetBytes, QIODevice::WriteOnly) << Packet(type, payload);
    //readIncommingData(packetBytes); //WARNING: debug loopback
    if(type == Packet::Control)
        emit sendData(TCP, packetBytes);
    else
        emit sendData(UDP, packetBytes);
}

void VoiceApp::updateUiToState(){
    QColor buttonColor;
    switch(m_state){
    case Disconnected:
        buttonColor.setNamedColor("#bbb");
        m_ui->call_bt->setIcon(QIcon(":/icons/call"));
        break;
    case Ready:
        buttonColor.setNamedColor("#0b0");
        m_ui->call_bt->setIcon(QIcon(":/icons/call"));
        break;
    case Calling:
        buttonColor.setNamedColor("#b00");
        m_ui->call_bt->setIcon(QIcon(":/icons/hangup"));
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

QDataStream& operator <<(QDataStream &out, const VoiceApp::Packet &packet){
    out << (quint8)packet.type;
    out << packet.payload;
    return out;
}

QDataStream& operator >>(QDataStream &in, VoiceApp::Packet &packet){
    quint8 typeInt;
    in >> typeInt;
    in >> packet.payload;
    packet.type = (VoiceApp::Packet::Type)typeInt;
    return in;
}
