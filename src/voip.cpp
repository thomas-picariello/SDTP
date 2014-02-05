#include "voip.h"

VoIP::VoIP(QIODevice *parent): QIODevice(parent){
    mOpusDecoder = new QOpusDecoder();
    mOpusEncoder = new QOpusEncoder();

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
    mAudioInput->setNotifyInterval((int)mOpusEncoder->getOpusFrameSize());
    connect(mAudioInput, SIGNAL(notify()),
            mOpusEncoder, SLOT(encode()));


    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    mAudioOutput = new QAudioOutput(format, this);
    mAudioOutput->setNotifyInterval((int)mOpusEncoder->getOpusFrameSize());

    connect(mOpusEncoder, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));

    /*debug*/
    connect(mAudioInput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(inputStateChanged(QAudio::State)));
    connect(mAudioOutput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(outputStateChanged(QAudio::State)));
    /**/
}

void VoIP::start(){
    mAudioInput->start(mOpusEncoder);
    //mAudioOutput->start(mOpusDecoder);
    setOpenMode(ReadWrite);
}

void VoIP::stop(){
    mAudioInput->stop();
    mAudioOutput->stop();
    setOpenMode(NotOpen);
}

qint64 VoIP::readData(char * data, qint64 maxSize){
    qint64 read = mOpusEncoder->read(data, maxSize);
    return read;
}

qint64 VoIP::writeData(const char * data, qint64 maxSize){
    if(mAudioOutput->state() != QAudio::ActiveState && openMode() == ReadWrite)
        mAudioOutput->start(mOpusDecoder);
    qint64 written = mOpusDecoder->write(data, maxSize);
    return written;
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
