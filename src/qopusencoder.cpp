#include "qopusencoder.h"

QOpusEncoder::QOpusEncoder(QAudioFormat inputFormat, float frameSizeInMs, QIODevice* parent):
    QIODevice(parent)
{
    mApplication = OPUS_APPLICATION_VOIP;
    mInputAudioFormat = inputFormat;
    mOpusFrameLength = frameSizeInMs;
    mBufferLength = 40.0; //for both buffers


    mOpusAudioFormat.setSampleRate(48000);
    mOpusAudioFormat.setChannelCount(2);
    mOpusAudioFormat.setCodec("audio/pcm");
    mOpusAudioFormat.setSampleSize(16);
    mOpusAudioFormat.setSampleType(QAudioFormat::SignedInt);

    int err = OPUS_OK;
    mEncoder = opus_encoder_create(mOpusAudioFormat.sampleRate(),
                                   mOpusAudioFormat.channelCount(),
                                   mApplication,
                                   &err);
    if(err != OPUS_OK)
        emit error(err);
    setOpenMode(ReadWrite);
}

bool QOpusEncoder::isSequential() const{
    return true;
}

QAudioFormat QOpusEncoder::getInputAudioFormat() const{
    return mInputAudioFormat;
}

void QOpusEncoder::setInputAudioFormat(QAudioFormat inputFormat){
    mInputAudioFormat = inputFormat;
}

float QOpusEncoder::getBufferMaxLength() const{
    return mBufferLength;
}

void QOpusEncoder::setBufferMaxLength(float lengthInMs){
    mBufferLength = lengthInMs;
}

qint64 QOpusEncoder::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mEncodedBuffer.size();
    qint64 i = 0;
    while(i<maxSize && i<bufferSize){
        data[i] = mEncodedBuffer.at(i);
        i++;
    }
    mEncodedBuffer.remove(0,i);
    return i;
}

qint64 QOpusEncoder::writeData(const char * data, qint64 maxSize){
    qint32 opusFrameSizeInBytes = mInputAudioFormat.bytesForDuration((int)mOpusFrameLength*1000);
    qint32 maxBufferLengthInBytes = mInputAudioFormat.bytesForDuration((int)mBufferLength*1000);

    mPcmBuffer.append(data, opusFrameSizeInBytes);

    if(mPcmBuffer.size() > maxBufferLengthInBytes){
        qDebug()<<"encoder pcm buffer trimmed !";
        mPcmBuffer.remove(0, mPcmBuffer.size()-maxBufferLengthInBytes);
    }
    return opusFrameSizeInBytes;
}

float QOpusEncoder::getOpusFrameSize() const{
    return mOpusFrameLength;
}

void QOpusEncoder::setOpusFrameSize(float frameSizeInMs){
    QList<int> authorisedValues;
    authorisedValues << 2.5 << 5.0 << 10.0 << 20.0 << 40.0 << 60.0;
    if(authorisedValues.contains(frameSizeInMs)){
        mOpusFrameLength = frameSizeInMs;
    }
}

int QOpusEncoder::getEncoderApplication() const{
    return mApplication;
}

void QOpusEncoder::setEncoderApplication(int application){
    QList<int> authorisedValues;
    authorisedValues << OPUS_APPLICATION_AUDIO
                     << OPUS_APPLICATION_RESTRICTED_LOWDELAY
                     << OPUS_APPLICATION_VOIP;
    if(authorisedValues.contains(application)){
        mApplication = application;
        opus_encoder_ctl(mEncoder, OPUS_SET_APPLICATION(application));
    }
}

quint64 QOpusEncoder::getBitrate() const{
    qint32 returnValue;
    opus_encoder_ctl(mEncoder, OPUS_GET_BITRATE(&returnValue));
    return returnValue;
}

void QOpusEncoder::setBitrate(quint64 bitrate){
    if(bitrate >= 500 && bitrate <= 512000){
        opus_encoder_ctl(mEncoder, OPUS_SET_BITRATE(bitrate));
    }
}

QString QOpusEncoder::getOpusErrorDesc(int errorCode){
    return QString(opus_strerror(errorCode));
}

void QOpusEncoder::encode(){
    qint32 pcmFrameLength = mInputAudioFormat.framesForDuration(mOpusFrameLength*1000);
    int pcmFrameSize = pcmFrameLength * mInputAudioFormat.bytesPerFrame()*mInputAudioFormat.channelCount();

    while(mPcmBuffer.size() >= pcmFrameSize){
        QVector<uchar> opusPacket(4000); //arbitrary packet max size

        int encodedBytes = opus_encode(mEncoder,
                                 reinterpret_cast<qint16*>(mPcmBuffer.left(pcmFrameSize).data()),
                                 pcmFrameLength,
                                 opusPacket.data(),
                                 4000);
        //remove the frame from the buffer
        mPcmBuffer.remove(0, pcmFrameSize);

        if(encodedBytes < 0){
            emit(error(encodedBytes));
        }else{
            for(int i=0; i<encodedBytes; i++){
                mEncodedBuffer.append(opusPacket[i]);
            }

        }
    }
    emit readyRead();
}

QOpusEncoder::~QOpusEncoder(){
    opus_encoder_destroy(mEncoder);
}
