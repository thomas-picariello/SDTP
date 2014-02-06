#include "qopusdecoder.h"

QOpusDecoder::QOpusDecoder(QAudioFormat outputFormat, float frameSizeInMs, QIODevice* parent):
    QIODevice(parent)
{
    mOpusFrameLength = frameSizeInMs;
    mBufferMaxSize = 16384; //in bytes
    mOutputAudioFormat = outputFormat;

    mOpusAudioFormat.setSampleRate(48000);
    mOpusAudioFormat.setChannelCount(2);
    mOpusAudioFormat.setCodec("audio/pcm");
    mOpusAudioFormat.setSampleSize(16);
    mOpusAudioFormat.setSampleType(QAudioFormat::SignedInt);

    int err = OPUS_OK;
    mDecoder = opus_decoder_create(mOpusAudioFormat.sampleRate(),
                                   mOpusAudioFormat.channelCount(),
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
    while(i<maxSize && i<bufferSize){
        data[i] = mPcmBuffer.at(i);
        i++;
    }
    mPcmBuffer.remove(0,i);
    return i;
}


qint64 QOpusDecoder::writeData(const char * data, qint64 maxSize){
    mEncodedBuffer.append(data, maxSize);
    if(mEncodedBuffer.size() > mBufferMaxSize){
        qDebug()<<"decoder encoded buffer trimmed !";
        qDebug()<<mEncodedBuffer.size();
        mEncodedBuffer.remove(0, mEncodedBuffer.size()-mBufferMaxSize);
    }
    qDebug()<<mEncodedBuffer.size();
    return maxSize;
}

QString QOpusDecoder::getOpusErrorDesc(int errorCode){
    return QString(opus_strerror(errorCode));
}

void QOpusDecoder::decode(){
    if(mEncodedBuffer.size()==0){
        qDebug()<<"nothing to decode !";
    }else{
        qint32 pcmFrameLength = mOutputAudioFormat.framesForDuration(mOpusFrameLength*1000);
        int pcmFrameSize = pcmFrameLength * mOutputAudioFormat.bytesPerFrame()*mOutputAudioFormat.channelCount();

        QVector<uchar> opusFrames(mEncodedBuffer.size());
        QVector<qint16> pcmData(pcmFrameSize);

        int i = 0;
        for(int i=0; i<mEncodedBuffer.size(); i++){
            opusFrames[i] = mEncodedBuffer.at(i);
        }
        mEncodedBuffer.resize(0);
        qDebug()<<opusFrames.size()<<"->"<<pcmData.size();

        int decodedFrames = opus_decode(mDecoder,
                             opusFrames.constData(),
                             opusFrames.size(),
                             pcmData.data(),
                             pcmFrameLength, //number of samples per channel per frame
                             0);
        if(decodedFrames < 0){
            emit error(0);
        }else{
            //TODO: trimm mPcmBuffer if needed
            mPcmBuffer.clear();
            for(int i=0; i<pcmData.size(); i++){
                char msb = (pcmData[i] >> 8) & 0xFF;
                char lsb = pcmData[i] & 0xFF;
                //TODO: try to inverse...
                mPcmBuffer.append(lsb);
                mPcmBuffer.append(msb);

            }
        }
    }
}

QOpusDecoder::~QOpusDecoder(){
    opus_decoder_destroy(mDecoder);
}
