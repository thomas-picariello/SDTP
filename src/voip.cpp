#include "voip.h"

VoIP::VoIP(QIODevice *parent):QIODevice(parent){
    mApplication = OPUS_APPLICATION_VOIP;
    mOpusFrameLength = 40.0;

    mAudioFormat.setChannelCount(1);
    mAudioFormat.setSampleRate(48000);
    mAudioFormat.setSampleSize(16);                        //Requiered by Opus
    mAudioFormat.setCodec("audio/pcm");                    //Requiered by Opus
    mAudioFormat.setByteOrder(QAudioFormat::LittleEndian); //Requiered by Opus
    mAudioFormat.setSampleType(QAudioFormat::SignedInt);   //Requiered by Opus

    QAudioDeviceInfo info;
    info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(mAudioFormat)) {
        qWarning() << "Default input format not supported, trying to use the nearest.";
        mAudioFormat = info.nearestFormat(mAudioFormat);
    }
    mAudioInput = new QAudioInput(mAudioFormat, this);

    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(mAudioFormat)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    mAudioOutput = new QAudioOutput(mAudioFormat, this);

    mAudioInput->setNotifyInterval((int)mOpusFrameLength);

    connect(mAudioInput, SIGNAL(notify()),
            this, SLOT(opusEncode()));

    //the input pcm buffer size is set to the duration of the opus frame
    mInputPcmBuffer.setMaxSize(mAudioFormat.framesForDuration((qint64)mOpusFrameLength*1000)*mAudioFormat.channelCount());
    //the input pcm buffer size is set to 2 time the duration of the opus frame (arbitrary)
    mOutputPcmBuffer.setMaxSize(mAudioFormat.framesForDuration(2*(qint64)mOpusFrameLength*1000)*mAudioFormat.channelCount());
    mInputPcmBuffer.open(ReadWrite);
    mOutputPcmBuffer.open(ReadWrite);

    int err = OPUS_OK;
    mEncoder = opus_encoder_create(mAudioFormat.sampleRate(), 2, mApplication, &err);
    if(err != OPUS_OK)
        displayOpusErr(err);
    mDecoder = opus_decoder_create(mAudioFormat.sampleRate(), 2, &err);
    if(err != OPUS_OK)
        displayOpusErr(err);

    setBitrate(25000);
}

void VoIP::start(){
    mAudioInput->start(&mInputPcmBuffer);
    setOpenMode(ReadWrite);
}

void VoIP::stop(){
    mAudioInput->stop();
    mAudioOutput->stop();
    setOpenMode(NotOpen);
}

QAudioFormat VoIP::getAudioFormat() const{
    return mAudioFormat;
}

void VoIP::setAudioFormat(QAudioFormat format){
    mAudioFormat = format;
}

float VoIP::getOpusFrameSize() const{
    return mOpusFrameLength;
}

void VoIP::setOpusFrameSize(float frameSizeInMs){
    QList<int> authorisedValues;
    authorisedValues << 2.5 << 5.0 << 10.0 << 20.0 << 40.0 << 60.0;
    if(authorisedValues.contains(frameSizeInMs)){
        mOpusFrameLength = frameSizeInMs;
    }
}

int VoIP::getEncoderApplication() const{
    return mApplication;
}

void VoIP::setEncoderApplication(int application){
    QList<int> authorisedValues;
    authorisedValues << OPUS_APPLICATION_AUDIO
                     << OPUS_APPLICATION_RESTRICTED_LOWDELAY
                     << OPUS_APPLICATION_VOIP;
    if(authorisedValues.contains(application)){
        mApplication = application;
        opus_encoder_ctl(mEncoder, OPUS_SET_APPLICATION(application));
    }
}

quint64 VoIP::getBitrate() const{
    qint32 returnValue;
    opus_encoder_ctl(mEncoder, OPUS_GET_BITRATE(&returnValue));
    return returnValue;
}

void VoIP::setBitrate(quint64 bitrate){
    if(bitrate >= 500 && bitrate <= 512000){
        opus_encoder_ctl(mEncoder, OPUS_SET_BITRATE(bitrate));
    }
}

void VoIP::displayOpusErr(int err){
    qWarning() << "Opus error:" << opus_strerror(err);
}
qint64 VoIP::readData(char * data, qint64 maxSize){
    qint64 pos = 0;
    while((pos < maxSize) && (pos <mInputEncodedBuffer.size())){
        data[pos] = mInputEncodedBuffer.at(pos);
        pos++;
    }
    mInputEncodedBuffer.remove(0, pos);
    return pos;
}

qint64 VoIP::writeData(const char * data, qint64 size){
    if(mAudioOutput->state() != QAudio::ActiveState && openMode() == ReadWrite)
        mAudioOutput->start(&mOutputPcmBuffer);
    mOutputEncodedBuffer.append(data, size);
    opusDecode();
    return size;
}

void VoIP::opusEncode(){
    QVector<uchar> encodedFrame(4000);
    int encodedBytes = opus_encode(mEncoder,
                                 mInputPcmBuffer.data(),
                                 mInputPcmBuffer.size()/2,
                                 encodedFrame.data(),
                                 4000);
    if(encodedBytes < 0){
        displayOpusErr(encodedBytes);
    }else{
        mInputEncodedBuffer.clear();
        for(int i=0; i<encodedBytes; i++)
            mInputEncodedBuffer.append(encodedFrame.at(i));
        emit readyRead();
    }
}

void VoIP::opusDecode(){
    static int pcmFrameLength = mAudioFormat.framesForDuration((qint64)mOpusFrameLength*1000)*mAudioFormat.channelCount();
    int decodedFrames = opus_decode(mDecoder,
                                 reinterpret_cast<const uchar*>(mOutputEncodedBuffer.constData()),
                                 mOutputEncodedBuffer.size(),
                                 mOutputPcmBuffer.preAllocate(pcmFrameLength),
                                 pcmFrameLength/2, //number of samples per channel per frame
                                 0);
    if(decodedFrames < 0)
        displayOpusErr(decodedFrames);
    mOutputEncodedBuffer.clear();
}

VoIP::~VoIP(){
    opus_decoder_destroy(mDecoder);
    opus_encoder_destroy(mEncoder);
}
