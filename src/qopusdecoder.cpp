#include "qopusdecoder.h"

QOpusDecoder::QOpusDecoder(QIODevice* parent): QIODevice(parent){
    mError = OPUS_OK;

    mAudioFormat.setSampleRate(48000);
    mAudioFormat.setChannelCount(2);
    mAudioFormat.setCodec("audio/pcm");
    mAudioFormat.setSampleSize(16);
    mAudioFormat.setSampleType(QAudioFormat::SignedInt);
}

bool QOpusDecoder::open(){
    mDecoder = opus_decoder_create(mAudioFormat.sampleRate(),
                                   mAudioFormat.channelCount(),
                                   &mError);
    if(mError){
        return false;
    }else{
        setOpenMode(ReadWrite);
        return true;
    }
}

void QOpusDecoder::close(){
    opus_decoder_destroy(mDecoder);
    setOpenMode(NotOpen);
}

bool QOpusDecoder::isSequential() const{
    return true;
}

qint64 QOpusDecoder::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mBuffer.size();
    qint64 i = 0;
    while(i<maxSize && i<bufferSize){
        data[i] = mBuffer.at(i);
        i++;
    }
    return i;
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

qint64 QOpusDecoder::writeData(const char * data, qint64 maxSize){
    mBuffer.clear();
    mBuffer.append(data, maxSize);
    emit readyRead();
    return mBuffer.size();

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

    /*mBuffer.clear();
    mBuffer.append(data, maxSize);
    emit readyRead();
    return mBuffer.size();*/
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

QOpusDecoder::~QOpusDecoder(){
    close();
}
