#include "voip.h"

VoIP::VoIP(QIODevice *parent): QIODevice(parent){
    mOpusDecoder = new QOpusDecoder();
    mOpusEncoder = new QOpusEncoder();

    QAudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(44000);
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

    /*debug*/
    connect(mAudioInput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(inputStateChanged(QAudio::State)));
    connect(mAudioOutput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(outputStateChanged(QAudio::State)));
    /**/
    connect(mOpusEncoder, SIGNAL(readyRead()),
            this, SLOT(startAudioOutput()));
}

void VoIP::start(){
    mOpusDecoder->open();
    mOpusEncoder->open();
    mAudioInput->start(mOpusDecoder);
    mAudioOutput->start(mOpusEncoder);
    setOpenMode(ReadWrite);
}

void VoIP::stop(){
    mOpusDecoder->close();
    mAudioInput->stop();
    mAudioOutput->stop();
    mOpusEncoder->close();
    setOpenMode(NotOpen);
}

/*void VoIP::startAudioOutput(){
    if(mCallState==ONLINE && mAudioOutput->state() != QAudio::ActiveState)
        mAudioOutput->start(mOpus);
}*/

qint64 VoIP::readData(char * data, qint64 maxSize){
    return maxSize;
}

qint64 VoIP::writeData(const char * data, qint64 maxSize){
    return maxSize;
}

/*debug*/
void VoIP::inputStateChanged(QAudio::State state){
    qDebug() << "New input state:" << state;
}

void VoIP::outputStateChanged(QAudio::State state){
    qDebug() << "New output state:" << state;
}
/**/
VoIP::~VoIP(){
    delete mOpusEncoder, mOpusDecoder, mAudioInput, mAudioOutput;
}
