#include "voip.h"

VoIP::VoIP(QIODevice *parent): QIODevice(parent){

    QAudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(48000);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian); //Requiered by Opus
    format.setSampleType(QAudioFormat::SignedInt);   //Requiered by Opus

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

    mOpusDecoder = new QOpusDecoder(format, this);
    mOpusEncoder = new QOpusEncoder(format, this);

//    mAudioInput->setNotifyInterval((int)mOpusEncoder->getOpusFrameSize());
//    mAudioOutput->setNotifyInterval((int)mOpusEncoder->getOpusFrameSize());

//    connect(mAudioOutput, SIGNAL(notify()),
//            mOpusDecoder, SLOT(decode()));
//    connect(mAudioInput, SIGNAL(notify()),
//            mOpusEncoder, SLOT(encode()));

    connect(mOpusEncoder, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    //Debug
    connect(mOpusDecoder, SIGNAL(error(int)),
            this, SLOT(decoderErr(int)));
    connect(mOpusEncoder, SIGNAL(error(int)),
            this, SLOT(encoderErr(int)));
}

void VoIP::start(){
    mAudioInput->start(mOpusEncoder);
    setOpenMode(ReadWrite);
}

void VoIP::stop(){
    mAudioInput->stop();
    mAudioOutput->stop();
    setOpenMode(NotOpen);
}

void VoIP::encoderErr(int err){
    qDebug() << "encoder error:" << mOpusEncoder->getOpusErrorDesc(err);
}

void VoIP::decoderErr(int err){
    qDebug() << "decoder error:" << mOpusDecoder->getOpusErrorDesc(err);
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
