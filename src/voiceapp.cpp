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

    connect(m_ui->output_vol_sl, &QSlider::valueChanged, this, &VoiceApp::setOutputVolume);
    connect(m_ui->mic_mute_bt, &QToolButton::clicked, this, &VoiceApp::setInputMute);
    connect(m_ui->output_mute_bt, &QToolButton::clicked, this, &VoiceApp::setOutputVolume);
}


void VoiceApp::readIncommingData(const QByteArray& data){
    m_codec.write(data);
}

void VoiceApp::endCall(){
    m_state = Ready;
    m_codec.stop();
    updateUiToState();
}

void VoiceApp::onContactStatusChange(){
    Contact* contact = dynamic_cast<Contact*>(sender());
    if(contact){
        updateUiToState();
    }
}

void VoiceApp::onCodecReadyRead(){
    emit sendData(TCP, m_codec.readAll());
}

void VoiceApp::startCall(){
    m_state = Calling;
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

void VoiceApp::updateUiToState(){
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
