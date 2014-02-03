#include "qopusdevice.h"

QOpusDevice::QOpusDevice(int frameSizeInMicrosecs, QIODevice* parent) :
    QIODevice(parent),
    mUnderlyingDevice(new QBuffer()),
    internalBufferFlag(true)
{
    initOpus();

    connect(mUnderlyingDevice, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    mUnderlyingDevice->open(ReadWrite);
    setOpenMode(ReadWrite);
}

QOpusDevice::QOpusDevice(QIODevice *deviceToUse, int frameSizeInMicrosecs, QIODevice* parent) :
    QIODevice(parent),
    mUnderlyingDevice(deviceToUse),
    internalBufferFlag(false)
{
    initOpus();

    connect(mUnderlyingDevice, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
}

void QOpusDevice::initOpus(){
    mError = OPUS_OK;
    mApplication = OPUS_APPLICATION_VOIP;

    mAudioFormat.setSampleRate(48000);
    mAudioFormat.setChannelCount(2);
    mAudioFormat.setCodec("audio/pcm");
    mAudioFormat.setSampleSize(16);
    mAudioFormat.setSampleType(QAudioFormat::SignedInt);

    //mEncoder = opus_encoder_create(mAudioFormat.sampleRate(), mAudioFormat.channelCount(), mApplication, &mError);
    //mDecoder = opus_decoder_create(mAudioFormat.sampleRate(), mAudioFormat.channelCount(), &mError);
}

bool QOpusDevice::open(){
    bool underlyingOpen;
    if(internalBufferFlag)
        underlyingOpen = mUnderlyingDevice->open(ReadWrite);
    else
        underlyingOpen = (mUnderlyingDevice->openMode() == ReadWrite);
    if(underlyingOpen)
        setOpenMode(ReadWrite);
    return underlyingOpen;
}

void QOpusDevice::close(){
    if(internalBufferFlag)
        mUnderlyingDevice->close();
    setOpenMode(NotOpen);
}

bool QOpusDevice::isSequential() const{
    return true;
}

bool QOpusDevice::hasInternalBuffer() const{
    return internalBufferFlag;
}

qint64 QOpusDevice::readData(char * data, qint64 maxSize){
    //QByteArray payload;
    qint64 readByteNumber = mUnderlyingDevice->read(data, maxSize);
    //if (readByteNumber == -1)
    //    return -1;
    //assuming that Opus reassemble the packets in an internal buffer
//    mError = 0;
//    mError = opus_decode(mDecoder,
//                (const unsigned char*)payload.constData(),
//                maxSize,
//                reinterpret_cast<qint16 *>(data),
//                mAudioFormat.framesForDuration(mOpusFrameSize)/mAudioFormat.channelCount(), //number of samples per channel per frame
//                0);

//    if(mError < 0) emit(error(mError));
    return readByteNumber;

    /*qint64 bufferSize = mBuffer.size();
    qint64 i = 0;
    while(i<maxSize && i<bufferSize){
        data[i] = mBuffer.at(i);
        i++;
    }
    return i;*/
}

qint64 QOpusDevice::writeData(const char * data, qint64 maxSize){
    qint64 processedByteCount = 0;
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
        processedByteCount = mUnderlyingDevice->write(data, maxSize);
    //}

    /*mBuffer.clear();
    mBuffer.append(data, maxSize);
    emit readyRead();
    return mBuffer.size();*/
    return processedByteCount;
}

int QOpusDevice::getFrameSize() const{
    return mOpusFrameSize;
}

void QOpusDevice::setFrameSize(int frameSizeInMicrosecs){
    QList<int> authorisedValues;
    authorisedValues << 25 << 50 << 100 << 200 << 400 << 600;
    if(authorisedValues.contains(frameSizeInMicrosecs)){
        mOpusFrameSize = frameSizeInMicrosecs;
    }
}

int QOpusDevice::getEncoderApplication() const{
    return mApplication;
}

void QOpusDevice::setEncoderApplication(int application){
    QList<int> authorisedValues;
    authorisedValues << OPUS_APPLICATION_AUDIO
                     << OPUS_APPLICATION_RESTRICTED_LOWDELAY
                     << OPUS_APPLICATION_VOIP;
    if(authorisedValues.contains(application)){
        mApplication = application;
        opus_encoder_ctl(mEncoder, OPUS_SET_APPLICATION(application));
    }
}

quint64 QOpusDevice::getBitrate() const{
    qint32 returnValue;
    opus_encoder_ctl(mEncoder, OPUS_GET_BITRATE(&returnValue));
    return returnValue;
}

void QOpusDevice::setBitrate(quint64 bitrate){
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

QOpusDevice::~QOpusDevice(){
    //opus_encoder_destroy(mEncoder);
    //opus_decoder_destroy(mDecoder);
    if(internalBufferFlag)
        delete mUnderlyingDevice;
}
