#include "qopusdevice.h"

QOpusDevice::QOpusDevice(QIODevice* deviceToUse, QObject* parent) :
    QIODevice(parent), mUnderlyingDevice(deviceToUse)
{
    mError = OPUS_OK;
    mFrameSizePerChannel = 960; //20ms at 48kHz
    opus_int32 sampleRate = 48000;
    mChannelsNumber = 2;
    mFrameSizeInByte = mFrameSizePerChannel * mChannelsNumber * sizeof(opus_int16);
    int application = OPUS_APPLICATION_VOIP;
    mEncoder = opus_encoder_create(sampleRate, mChannelsNumber, application, &mError);
    mDecoder = opus_decoder_create(sampleRate, mChannelsNumber, &mError);
}

bool QOpusDevice::open(OpenMode mode){
    bool underlyingOk;
    if (mUnderlyingDevice->isOpen()){
        underlyingOk = (mUnderlyingDevice->openMode() != mode);
    }else{
        underlyingOk = mUnderlyingDevice->open(mode);
    }
    if (underlyingOk){
        setOpenMode(mode);
        return true;
    }
    return false;
}

void QOpusDevice::close(){
    mUnderlyingDevice->close();
    setOpenMode(NotOpen);
}

bool QOpusDevice::isSequential() const{
    return true;
}

qint64 QOpusDevice::readData(char * data, qint64 maxSize){
    qint64 readByteNumber = mUnderlyingDevice->read(data, maxSize);
    if (readByteNumber == -1)
        return -1;
    mOutputBuffer.append(data);
    if(mOutputBuffer.size() >= mFrameSizeInByte){
        //extract the frame
        QByteArray frame = mOutputBuffer.left(mFrameSizeInByte);
        //remove the frame from the buffer
        mOutputBuffer = mOutputBuffer.right(mFrameSizeInByte);
        //decode the frame
        opus_decode(mDecoder,
                    (const unsigned char*)frame.constData(),
                    mFrameSizePerChannel,
                    (qint16 *)data,
                    mFrameSizePerChannel,
                    0);
        return mFrameSizePerChannel;
    }
    return 0;
}

qint64 QOpusDevice::writeData(const char * input, qint64 maxSize){
    qint64 encodedByteNumber = 0;
    QByteArray encodedPayload;
    mInputBuffer.append(input, maxSize);
    if(mInputBuffer.size() >= mFrameSizePerChannel){
        //extract the frame
        QByteArray frame = mInputBuffer.left(mFrameSizePerChannel);
        //remove the frame from the buffer
        mInputBuffer = mInputBuffer.right(mFrameSizePerChannel);
        //encode the frame
        opus_encode(mEncoder,
                    (const opus_int16*)frame.data(),
                    mFrameSizePerChannel,
                    (unsigned char*)encodedPayload.data(),
                    4000);
        //write the encoded frame (payload) to the output
        encodedByteNumber = mUnderlyingDevice->write(encodedPayload.data(), maxSize);
    }
    return encodedByteNumber;
}

QOpusDevice::~QOpusDevice(){
    opus_encoder_destroy(mEncoder);
    opus_decoder_destroy(mDecoder);
    delete mUnderlyingDevice;
}
