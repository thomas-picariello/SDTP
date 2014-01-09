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

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    mAudioInput = new QAudioInput(format);
    //mBuffer = new QBuffer();
    //mBuffer->open(QBuffer::ReadWrite | QBuffer::Truncate);
    //mAudioInput->start(mBuffer);
}


void VoIP::call(Contact contact){

}

void VoIP::endCall(){

}

VoIP::~VoIP(){

}
