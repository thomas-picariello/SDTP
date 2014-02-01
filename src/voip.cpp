#include "voip.h"

VoIP::VoIP(QObject *parent):
    QObject(parent),
    mCallState(OFFLINE),
    mOpus(new QOpusDevice())
{
    initAudio();
}

VoIP::VoIP(QIODevice *interfaceIODevice, QObject *parent) :
    QObject(parent),
    mCallState(OFFLINE),
    mOpus(new QOpusDevice(interfaceIODevice))
{
    initAudio();
}

void VoIP::initAudio(){
    QAudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(48000);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian); //Requiered by Opus
    format.setSampleType(QAudioFormat::SignedInt); //Requiered by Opus (or float if using encode_float)

    QAudioDeviceInfo info;
    info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default input format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    mAudioInput = new QAudioInput(format, this);

    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    mAudioOutput = new QAudioOutput(format, this);

    connect(mAudioInput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(inputStateChanged(QAudio::State)));
    connect(mAudioOutput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(outputStateChanged(QAudio::State)));
    connect(mOpus, SIGNAL(readyRead()),
            this, SLOT(startAudioOutput()));
}

void VoIP::call(const Contact &contact){
    mCallState = ONLINE;
    emit callStateChanged(mCallState);
    mOpus->open();
    mAudioInput->start(mOpus);
}

void VoIP::endCall(){
    mAudioInput->stop();
    mAudioOutput->stop();
    mOpus->close();
    mCallState = OFFLINE;
    emit callStateChanged(mCallState);
}

QOpusDevice* VoIP::getOpusDevice(){
    return mOpus;
}

VoIP::CallState VoIP::getCallState(){
    return mCallState;
}

void VoIP::takeIncommingCall(){
    mCallState= ONLINE;
    emit callStateChanged(mCallState);
    mOpus->open();
    mAudioInput->start(mOpus);
    //delete previously allocated QOpusDevice instance
    //delete mOpus;
    //initialize a new one with the data interface
    //mOpus = new QOpusDevice(dataInterface);
}

void VoIP::startAudioOutput(){
    if(mCallState==ONLINE && mAudioOutput->state() != QAudio::ActiveState)
        mAudioOutput->start(mOpus);
}

//Debug
void VoIP::inputStateChanged(QAudio::State state){
    qDebug() << "New input state:" << state;
}

void VoIP::outputStateChanged(QAudio::State state){
    qDebug() << "New output state:" << state;
}

void VoIP::bufferWritten(qint64 bc){
    qDebug() << "Written to Buffer:" << bc << "Bytes";
}

VoIP::~VoIP(){
    delete mOpus, mAudioInput, mAudioOutput;
}
