#include "qopusencoder.h"

QOpusEncoder::QOpusEncoder(QAudioFormat inputFormat, QIODevice* parent):
    QIODevice(parent),
    mApplication(OPUS_APPLICATION_VOIP),
    mInputAudioFormat(inputFormat),
    mOpusFrameLength(20.0)
{
    int err = OPUS_OK;
    mEncoder = opus_encoder_create(48000,
                                   2,
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

qint64 QOpusEncoder::writeData(const char * data, qint64 size){
    static qint32 opusFrameLength = mInputAudioFormat.framesForDuration((qint64)mOpusFrameLength * 1000);
    static qint32 samplesPerOpusFrame = opusFrameLength * mInputAudioFormat.channelCount();
    static quint8 sampleSize = mInputAudioFormat.sampleSize()/8;

    quint64 bytesProcessed = 0;

    //if an entire opus frame is available,
    if(size >= samplesPerOpusFrame*sampleSize){
        const uchar *sample_ptr = reinterpret_cast<const uchar*>(data);
        //empty the buffer
        mPcmBuffer.clear();
        //perform the copy
        for(int i=0; i< samplesPerOpusFrame; i++){
            qint16 sample = qFromLittleEndian<qint16>(sample_ptr);
            mPcmBuffer.append(sample);
            sample_ptr += sampleSize;
            bytesProcessed += sampleSize;
        }
        qDebug()<<mPcmBuffer.size();
        //and encode the frame
        mEncodedBuffer.clear();
        mEncodedBuffer.resize(4000);
        int encodedBytes = opus_encode(mEncoder,
                                 mPcmBuffer.constData(),
                                 mPcmBuffer.size(),
                                 mEncodedBuffer.data(),
                                 4000); //arbitrary packet max size
        if(encodedBytes < 0)
            emit(error(encodedBytes));
        else
            mEncodedBuffer.resize(encodedBytes);
        qDebug()<<encodedBytes;
    }
    emit readyRead();
    return bytesProcessed;
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

QOpusEncoder::~QOpusEncoder(){
    opus_encoder_destroy(mEncoder);
}
