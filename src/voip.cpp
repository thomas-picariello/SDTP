#include "voip.h"

VoIP::VoIP(QObject *parent) :
    QObject(parent)
{
    mOpusIODevice = new QOpusDevice();
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(44100);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian); //Requiered by Opus
    format.setSampleType(QAudioFormat::SignedInt); //Requiered by Opus (or float if using encode_float)

    QAudioDeviceInfo info;
    info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    mAudioInput = new QAudioInput(format);

    info = QAudioDeviceInfo::defaultOutputDevice();
    if(info.isFormatSupported(format)){
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    mAudioOutput = new QAudioOutput(format);
}

void VoIP::call(Contact contact){
    mOpusIODevice->open(QIODevice::ReadWrite | QIODevice::Truncate);
    mAudioInput->start(mOpusIODevice);
    mAudioOutput->start(mOpusIODevice);
}

void VoIP::endCall(){
    mAudioInput->stop();
    mAudioOutput->stop();
    mOpusIODevice->close();
}

QOpusDevice* VoIP::getOpusIODevice(){
    return mOpusIODevice;
}

void VoIP::takeIncommingCall(QIODevice* dataInterface){
    //delete previously initalised Opus interface
    delete mOpusIODevice;
    //initialize a new one with the data interface
    mOpusIODevice = new QOpusDevice(dataInterface);
}

VoIP::~VoIP(){
    delete mOpusIODevice, mAudioInput, mAudioOutput;
}
