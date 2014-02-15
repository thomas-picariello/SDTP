#include "qpcmbuffer.h"

QPcmBuffer::QPcmBuffer(QIODevice* parent):QIODevice(parent){
    hasExternalBuffer = false;
    mBuffer = new QVector<qint16>;
}

QPcmBuffer::QPcmBuffer(qint64 bufferMaxSize, QIODevice* parent): QIODevice(parent){
    hasExternalBuffer = false;
    mBuffer = new QVector<qint16>;
    mBufferMaxSize = bufferMaxSize;
}

QPcmBuffer::QPcmBuffer(QVector<qint16> *buffer, QIODevice *parent): QIODevice(parent){
    hasExternalBuffer = true;
    mBuffer = buffer;
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

qint64 QPcmBuffer::writeData(const char * data, qint64 size){
    qint64 bytesProcessed = 0;
    const uchar *sample_ptr = reinterpret_cast<const uchar*>(data);
    qint16 sample;
    free(size);
    while((bytesProcessed < size) && (bytesProcessed/2 < mBufferMaxSize)){
        sample = qFromLittleEndian<qint16>(sample_ptr);
        mBuffer->append(sample);
        sample_ptr += 2;
        bytesProcessed += 2;
    }
    return bytesProcessed;
}

QVector<qint16>& QPcmBuffer::buffer(){
    return *mBuffer;
}

void QPcmBuffer::setBuffer(QVector<qint16> *buffer){
    if(openMode() == NotOpen){
        hasExternalBuffer = true;
        mBuffer = buffer;
    }
}

qint16* QPcmBuffer::preAllocate(qint64 size){
    free(size);
    qint16 *data_ptr = mBuffer->data()+mBuffer->size();
    mBuffer->resize(size);
    return data_ptr;
}

qint16* QPcmBuffer::data(){
    return mBuffer->data();
}

const qint16* QPcmBuffer::data() const{
    return mBuffer->data();
}

void QPcmBuffer::setData(const QVector<qint16> &data){
    for(int i=0; i<mBuffer->size() && i<data.size(); i++)
        mBuffer->data()[i] = data[i];
}

qint64 QPcmBuffer::size() const{
    return mBuffer->size();
}

qint64 QPcmBuffer::getMaxSize() const{
    return mBufferMaxSize;
}

void QPcmBuffer::setMaxSize(qint64 maxSize){
    mBufferMaxSize = maxSize;
}

void QPcmBuffer::free(qint64 size){
    if(mBuffer->size()+size > mBufferMaxSize){
        if(size > mBufferMaxSize)
            mBuffer->clear();
        else
            mBuffer->remove(0, mBuffer->size()+size - mBufferMaxSize);
        qDebug()<<"removed from buffer:"<<mBuffer->size()+size-mBufferMaxSize;
    }
}

QPcmBuffer::~QPcmBuffer(){
    if(!hasExternalBuffer)
        delete mBuffer;
}
