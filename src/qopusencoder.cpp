#include "qopusencoder.h"

QOpusEncoder::QOpusEncoder(float frameSizeInMs, QIODevice* parent): QIODevice(parent){
    mError = OPUS_OK;
    mApplication = OPUS_APPLICATION_VOIP;
    mOpusFrameSize = frameSizeInMs;
    mBufferLength = 40.0;

    mAudioFormat.setSampleRate(48000);
    mAudioFormat.setChannelCount(2);
    mAudioFormat.setCodec("audio/pcm");
    mAudioFormat.setSampleSize(16);
    mAudioFormat.setSampleType(QAudioFormat::SignedInt);

    mEncoder = opus_encoder_create(mAudioFormat.sampleRate(),
                                   mAudioFormat.channelCount(),
                                   mApplication,
                                   &mError);
    setOpenMode(ReadWrite);
}

bool QOpusEncoder::isSequential() const{
    return true;
}

float QOpusEncoder::getBufferLength() const{
    return mBufferLength;
}

void QOpusEncoder::setBufferLength(float lengthInMs){
    mBufferLength = lengthInMs;
}

qint64 QOpusEncoder::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mRawBuffer.size();
    qint64 i = 0;
    while(i<maxSize && i<bufferSize){
        data[i] = mRawBuffer.at(i);
        i++;
    }
    mRawBuffer.remove(0,i);
    return i;
}

qint64 QOpusEncoder::writeData(const char * data, qint64 maxSize){
    qint32 opusFrameSizeInBytes = mAudioFormat.bytesForDuration((int)mOpusFrameSize*1000);
    qint32 maxBufferLengthInBytes = mAudioFormat.bytesForDuration((int)mBufferLength*1000);
    mRawBuffer.append(data, opusFrameSizeInBytes);
    if(mRawBuffer.size() > maxBufferLengthInBytes){
        qDebug()<<"encoder buffer trimmed !";
        mRawBuffer.remove(0, mRawBuffer.size()-maxBufferLengthInBytes);
    }
    return opusFrameSizeInBytes;
}

float QOpusEncoder::getOpusFrameSize() const{
    return mOpusFrameSize;
}

void QOpusEncoder::setOpusFrameSize(float frameSizeInMs){
    QList<int> authorisedValues;
    authorisedValues << 2.5 << 5.0 << 10.0 << 20.0 << 40.0 << 60.0;
    if(authorisedValues.contains(frameSizeInMs)){
        mOpusFrameSize = frameSizeInMs;
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

static QString getOpusErrorDesc(int errorCode){
    switch(errorCode){
    case OPUS_OK:
        return "No error";
    case OPUS_BAD_ARG:
        return "One or more invalid/out of range arguments";
    case OPUS_BUFFER_TOO_SMALL:
        return "The mode struct passed is invalid";
    case OPUS_INTERNAL_ERROR:
        return "An internal error was detected";
    case OPUS_INVALID_PACKET:
        return "The compressed data passed is corrupted";
    case OPUS_UNIMPLEMENTED:
        return "Invalid/unsupported request number";
    case OPUS_INVALID_STATE:
        return "An encoder or decoder structure is invalid or already freed";
    case OPUS_ALLOC_FAIL:
        return "Memory allocation has failed";
    default:
        return "Unknown error code";
    }
}

void QOpusEncoder::encode(){
    qDebug()<<"encode...";
    //QByteArray payload;
    //mInputBuffer.append(input, maxSize);
    //qDebug() << mAudioFormat.bytesForDuration(mOpusFrameSize);
    //if(mInputBuffer.size() >= mAudioFormat.bytesForDuration(mOpusFrameSize)){
        //extract the frame
        //QByteArray frame = mInputBuffer.left(mAudioFormat.bytesForDuration(mOpusFrameSize));
        //remove the frame from the buffer
        //mInputBuffer = mInputBuffer.right(mAudioFormat.bytesForDuration(mOpusFrameSize));
        //encode the frame
//        mError = 0;
//        mError = opus_encode(mEncoder,
//                    (const opus_int16*)frame.data(),
//                    mAudioFormat.framesForDuration(mOpusFrameSize)/mAudioFormat.channelCount(), //number of samples per channel per frame
//                    (unsigned char*)payload.data(),
//                    4000);

//        if(mError < 0) emit(error(mError));
        //write the encoded frame (payload) to the output
    //}
    emit readyRead();
}

QOpusEncoder::~QOpusEncoder(){
    opus_encoder_destroy(mEncoder);
}
