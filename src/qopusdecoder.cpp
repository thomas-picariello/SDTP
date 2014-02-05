#include "qopusdecoder.h"

QOpusDecoder::QOpusDecoder(QIODevice* parent): QIODevice(parent){
    mError = OPUS_OK;
    mBufferMaxSize = 16384; //in bytes

    mAudioFormat.setSampleRate(48000);
    mAudioFormat.setChannelCount(2);
    mAudioFormat.setCodec("audio/pcm");
    mAudioFormat.setSampleSize(16);
    mAudioFormat.setSampleType(QAudioFormat::SignedInt);

    mDecoder = opus_decoder_create(mAudioFormat.sampleRate(),
                                   mAudioFormat.channelCount(),
                                   &mError);
    setOpenMode(ReadWrite);
}

bool QOpusDecoder::isSequential() const{
    return true;
}

quint64 QOpusDecoder::getBufferMaxSize() const{
    return mBufferMaxSize;
}

void QOpusDecoder::setBufferMaxSize(quint64 bytesCount){
    mBufferMaxSize = bytesCount;
}

qint64 QOpusDecoder::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mEncodedBuffer.size();
    qint64 i = 0;
    while(i<maxSize && i<bufferSize){
        data[i] = mEncodedBuffer.at(i);
        i++;
    }
    mEncodedBuffer.remove(0,i);
    return i;
}

qint64 QOpusDecoder::writeData(const char * data, qint64 maxSize){
    mEncodedBuffer.append(data, maxSize);
    if(mEncodedBuffer.size() > mBufferMaxSize){
        qDebug()<<"decoder buffer trimmed !";
        mEncodedBuffer.remove(0, mEncodedBuffer.size()-mBufferMaxSize);
    }
    decode();
    return maxSize;
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

void QOpusDecoder::decode(){
    qDebug()<<"decode...";
    //assuming that Opus reassemble the packets in an internal buffer
//    mError = 0;
//    mError = opus_decode(mDecoder,
//                (const unsigned char*)payload.constData(),
//                maxSize,
//                reinterpret_cast<qint16 *>(data),
//                mAudioFormat.framesForDuration(mOpusFrameSize)/mAudioFormat.channelCount(), //number of samples per channel per frame
//                0);

//    if(mError < 0) emit(error(mError));
}

QOpusDecoder::~QOpusDecoder(){
    opus_decoder_destroy(mDecoder);
}
