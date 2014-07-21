#include "opusvoicecodec.h"

OpusVoiceCodec::OpusVoiceCodec(QIODevice *parent):QIODevice(parent){
    m_application = OPUS_APPLICATION_VOIP;
    m_opusFrameLength = 40.0;

    m_audioFormat.setChannelCount(1);
    m_audioFormat.setSampleRate(48000);
    m_audioFormat.setSampleSize(16);                        //Requiered by Opus
    m_audioFormat.setCodec("audio/pcm");                    //Requiered by Opus
    m_audioFormat.setByteOrder(QAudioFormat::LittleEndian); //Requiered by Opus
    m_audioFormat.setSampleType(QAudioFormat::SignedInt);   //Requiered by Opus

    QAudioDeviceInfo info;
    info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(m_audioFormat)) {
        qWarning() << "Default input format not supported, trying to use the nearest.";
        m_audioFormat = info.nearestFormat(m_audioFormat);
    }
    m_audioInput = new QAudioInput(m_audioFormat, this);

    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(m_audioFormat)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    m_audioOutput = new QAudioOutput(m_audioFormat, this);

    m_audioInput->setNotifyInterval((int)m_opusFrameLength);

    connect(m_audioInput, SIGNAL(notify()),
            this, SLOT(opusEncode()));

    //the input pcm buffer size is set to the duration of the opus frame
    m_inputPcmBuffer.setMaxSize(m_audioFormat.framesForDuration((qint64)m_opusFrameLength*1000)*m_audioFormat.channelCount());
    //the input pcm buffer size is set to 2 time the duration of the opus frame (arbitrary)
    m_outputPcmBuffer.setMaxSize(m_audioFormat.framesForDuration(2*(qint64)m_opusFrameLength*1000)*m_audioFormat.channelCount());
    m_inputPcmBuffer.open(ReadWrite);
    m_outputPcmBuffer.open(ReadWrite);

    int err = OPUS_OK;
    m_encoder = opus_encoder_create(m_audioFormat.sampleRate(), 2, m_application, &err);
    if(err != OPUS_OK)
        displayOpusErr(err);
    m_decoder = opus_decoder_create(m_audioFormat.sampleRate(), 2, &err);
    if(err != OPUS_OK)
        displayOpusErr(err);
}

void OpusVoiceCodec::start(){
    m_audioInput->start(&m_inputPcmBuffer);
    setOpenMode(ReadWrite);
}

void OpusVoiceCodec::stop(){
    m_audioInput->stop();
    m_audioOutput->stop();
    setOpenMode(NotOpen);
}

QAudioFormat OpusVoiceCodec::getAudioFormat() const{
    return m_audioFormat;
}

void OpusVoiceCodec::setAudioFormat(QAudioFormat format){
    m_audioFormat = format;
}

float OpusVoiceCodec::getOpusFrameSize() const{
    return m_opusFrameLength;
}

void OpusVoiceCodec::setOpusFrameSize(float frameSizeInMs){
    QList<int> authorisedValues;
    authorisedValues << 2.5 << 5.0 << 10.0 << 20.0 << 40.0 << 60.0;
    if(authorisedValues.contains(frameSizeInMs)){
        m_opusFrameLength = frameSizeInMs;
        //the input pcm buffer size is set to the duration of the opus frame
        m_inputPcmBuffer.setMaxSize(m_audioFormat.framesForDuration((qint64)m_opusFrameLength*1000)*m_audioFormat.channelCount());
        //the input pcm buffer size is set to 2 time the duration of the opus frame (arbitrary)
        m_outputPcmBuffer.setMaxSize(m_audioFormat.framesForDuration(2*(qint64)m_opusFrameLength*1000)*m_audioFormat.channelCount());
    }
}

int OpusVoiceCodec::getEncoderApplication() const{
    return m_application;
}

void OpusVoiceCodec::setEncoderApplication(int application){
    QList<int> authorisedValues;
    authorisedValues << OPUS_APPLICATION_AUDIO
                     << OPUS_APPLICATION_RESTRICTED_LOWDELAY
                     << OPUS_APPLICATION_VOIP;
    if(authorisedValues.contains(application)){
        m_application = application;
        opus_encoder_ctl(m_encoder, OPUS_SET_APPLICATION(application));
    }
}

quint64 OpusVoiceCodec::getBitrate() const{
    qint32 returnValue;
    opus_encoder_ctl(m_encoder, OPUS_GET_BITRATE(&returnValue));
    return returnValue;
}

void OpusVoiceCodec::setBitrate(quint64 bitrate){
    if(bitrate >= 500 && bitrate <= 512000){
        opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(bitrate));
    }
}

void OpusVoiceCodec::setOutputVolume(qreal volume){
    m_audioOutput->setVolume(volume);
}

void OpusVoiceCodec::setInputMute(bool mute){
    if(mute)
        m_audioInput->suspend();
    else
        m_audioInput->resume();
}

void OpusVoiceCodec::setOutputMute(bool mute){
    if(mute)
        m_audioOutput->setVolume(0.0);
    else
        m_audioOutput->setVolume(1.0);
}

void OpusVoiceCodec::displayOpusErr(int err){
    qWarning() << "Opus error:" << opus_strerror(err);
}
qint64 OpusVoiceCodec::readData(char * data, qint64 maxSize){
    qint64 pos = 0;
    while((pos < maxSize) && (pos <m_inputEncodedBuffer.size())){
        data[pos] = m_inputEncodedBuffer.at(pos);
        pos++;
    }
    m_inputEncodedBuffer.remove(0, pos);
    return pos;
}

qint64 OpusVoiceCodec::writeData(const char * data, qint64 size){
    if(m_audioOutput->state() != QAudio::ActiveState && openMode() == ReadWrite)
        m_audioOutput->start(&m_outputPcmBuffer);
    m_outputEncodedBuffer.append(data, size);
    opusDecode();
    emit newOutputProbe(computeRMS(m_outputPcmBuffer, m_audioOutput->volume()));
    return size;
}

qint16 OpusVoiceCodec::computeRMS(const QPcmBuffer& buffer, qreal volume){
    qreal quadraticSum = 0;
    for(int i=0; i<buffer.size(); i++){
        quadraticSum += qPow(buffer.data()[i], 2);
    }
    return qSqrt(quadraticSum/buffer.size()) * volume;
}

void OpusVoiceCodec::opusEncode(){
    QVector<uchar> encodedFrame(4000);
    int encodedBytes = opus_encode(m_encoder,
                                 m_inputPcmBuffer.data(),
                                 m_inputPcmBuffer.size()/2,
                                 encodedFrame.data(),
                                 4000);
    if(encodedBytes < 0){
        displayOpusErr(encodedBytes);
    }else{
        m_inputEncodedBuffer.clear();
        for(int i=0; i<encodedBytes; i++)
            m_inputEncodedBuffer.append(encodedFrame.at(i));
        emit readyRead();
        emit newInputProbe(computeRMS(m_inputPcmBuffer, 1.0));
    }
}

void OpusVoiceCodec::opusDecode(){
    static int pcmFrameLength = m_audioFormat.framesForDuration((qint64)m_opusFrameLength*1000)*m_audioFormat.channelCount();
    int decodedFrames = opus_decode(m_decoder,
                                 reinterpret_cast<const uchar*>(m_outputEncodedBuffer.constData()),
                                 m_outputEncodedBuffer.size(),
                                 m_outputPcmBuffer.preAllocate(pcmFrameLength),
                                 pcmFrameLength/2, //number of samples per channel per frame
                                 0);
    if(decodedFrames < 0)
        displayOpusErr(decodedFrames);
    m_outputEncodedBuffer.clear();
}

OpusVoiceCodec::~OpusVoiceCodec(){
    opus_decoder_destroy(m_decoder);
    opus_encoder_destroy(m_encoder);
}
