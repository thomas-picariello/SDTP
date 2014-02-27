#include "qpcmbuffer.h"

QPcmBuffer::QPcmBuffer(QObject *parent): QIODevice(parent){
    hasExternalBuffer = false;
    mBuffer = new QVector<qint16>;
    mBufferMaxSize = 0;
}

QPcmBuffer::QPcmBuffer(qint64 maxSize, QObject* parent): QIODevice(parent){
    hasExternalBuffer = false;
    mBuffer = new QVector<qint16>;
    mBufferMaxSize = maxSize;
}

QPcmBuffer::QPcmBuffer(QVector<qint16> *buffer, QObject *parent): QIODevice(parent){
    hasExternalBuffer = true;
    mBuffer = buffer;
    mBufferMaxSize = 0;
}

QPcmBuffer::~QPcmBuffer(){
    if(!hasExternalBuffer)
        delete mBuffer;
}

QVector<qint16>& QPcmBuffer::buffer(){
    return *mBuffer;
}

qint16* QPcmBuffer::data(){
    return mBuffer->data();
}

const qint16* QPcmBuffer::data() const{
    return mBuffer->data();
}

void QPcmBuffer::free(qint64 size){
    if(mBuffer->size()+size > mBufferMaxSize && (mBufferMaxSize != 0)){
        if(size > mBufferMaxSize)
            mBuffer->clear();
        else
            mBuffer->remove(0, mBuffer->size()+size - mBufferMaxSize);
    }
}

qint64 QPcmBuffer::getMaxSize() const{
    return mBufferMaxSize;
}

qint16* QPcmBuffer::preAllocate(qint64 sampleCount){
    free(sampleCount);
    qint64 offset = mBuffer->size();
    mBuffer->resize(mBuffer->size()+sampleCount);
    return mBuffer->data()+offset;
}

qint64 QPcmBuffer::readData(char * data, qint64 maxSize){
    qint64 bufferSize = mBuffer->size();
    qint64 i = 0;
    while(i<(maxSize/2) && i<bufferSize){
        data[i*2] = mBuffer->at(i) & 0xFF;
        data[i*2+1] = (mBuffer->at(i) >> 8) & 0xFF;
        i++;
    }
    mBuffer->remove(0,i);
    return i*2;
}

void QPcmBuffer::setBuffer(QVector<qint16> *buffer){
    if(openMode() == NotOpen){
        if(!hasExternalBuffer)
            delete mBuffer;
        hasExternalBuffer = true;
        mBuffer = buffer;
    }
}

void QPcmBuffer::setData(const QVector<qint16> &data){
    for(int i=0; i<mBuffer->size() && i<data.size(); i++)
        mBuffer->data()[i] = data[i];
}

void QPcmBuffer::setMaxSize(qint64 maxSize){
    mBufferMaxSize = maxSize;
}

qint64 QPcmBuffer::size() const{
    return mBuffer->size();
}

qint64 QPcmBuffer::writeData(const char * data, qint64 size){
    qint64 bytesProcessed = 0;
    const uchar *sample_ptr = reinterpret_cast<const uchar*>(data);
    qint16 sample;
    free(size);
    while((bytesProcessed < size)){
        if((mBufferMaxSize > 0) && (bytesProcessed/2 > mBufferMaxSize))
            break;
        sample = qFromLittleEndian<qint16>(sample_ptr);
        mBuffer->append(sample);
        sample_ptr += 2;
        bytesProcessed += 2;
    }
    return bytesProcessed;
}
