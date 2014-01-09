#include "voip.h"

VoIP::VoIP(QObject *parent) :
    QObject(parent)
{
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(44100);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);


    QAudioDeviceInfo info;
    info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }

    info = QAudioDeviceInfo::defaultOutputDevice();
    if(info.isFormatSupported(format)){
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    mAudioOutput = new QAudioOutput(format);
    mAudioInput = new QAudioInput(format);
    mOpusIODevice = new QOpusDevice();
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

void takeIncommingCall(QIODevice* dataInterface){

}

VoIP::~VoIP(){
    delete mOpusIODevice, mAudioInput, mAudioOutput;
}
