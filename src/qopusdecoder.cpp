#include "qopusdecoder.h"

QOpusDecoder::QOpusDecoder(QAudioFormat outputFormat, QIODevice* parent):
    QIODevice(parent),
    mOpusFrameLength(20.0),
    mBufferMaxSize(4000), //in bytes (is the largest opus packet size set)
    mOutputAudioFormat(outputFormat)
{
    int err = OPUS_OK;
    mDecoder = opus_decoder_create(48000,
                                   2,
                                   &err);
    if(err != OPUS_OK)
        emit error(err);
    setOpenMode(ReadWrite);
}

bool QOpusDecoder::isSequential() const{
    return true;
}

QAudioFormat QOpusDecoder::getOutputAudioFormat() const{
    return mOutputAudioFormat;
}

void QOpusDecoder::setOutputAudioFormat(QAudioFormat outputFormat){
    mOutputAudioFormat = outputFormat;
}

quint64 QOpusDecoder::getBufferMaxSize() const{
    return mBufferMaxSize;
}

void QOpusDecoder::setBufferMaxSize(quint64 bytesCount){
    mBufferMaxSize = bytesCount;
}

qint64 QOpusDecoder::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mPcmBuffer.size();
    qint64 i = 0;
    while(i<(maxSize/2) && i<bufferSize){
        data[i*2] = mPcmBuffer.at(i) & 0xFF;
        data[i*2+1] = (mPcmBuffer.at(i) >> 8) & 0xFF;
        i++;
    }
    return i*2;
}

qint64 QOpusDecoder::writeData(const char * data, qint64 size){
    const uchar *data_ptr = reinterpret_cast<const uchar*>(data);

    mEncodedBuffer.clear();
    mEncodedBuffer.reserve(mBufferMaxSize);
    int i = 0;
    while(i<size && i<mBufferMaxSize){
        mEncodedBuffer.append(data_ptr[i]);
        i++;
    }
    decode();
    return i;
}

QString QOpusDecoder::getOpusErrorDesc(int errorCode){
    return QString(opus_strerror(errorCode));
}

void QOpusDecoder::decode(){
    if(mEncodedBuffer.size()==0){
        qDebug()<<"nothing to decode !";
    }else{
        qint32 pcmFrameLength = mOutputAudioFormat.framesForDuration(mOpusFrameLength*1000);
        mPcmBuffer.clear();
        mPcmBuffer.resize(pcmFrameLength*4);
        int decodedFrames = opus_decode(mDecoder,
                             mEncodedBuffer.constData(),
                             mEncodedBuffer.size(),
                             mPcmBuffer.data(),
                             pcmFrameLength*2, //number of samples per channel per frame
                             0);
        if(decodedFrames < 0)
            emit error(decodedFrames);
    }
}

QOpusDecoder::~QOpusDecoder(){
    opus_decoder_destroy(mDecoder);
}
