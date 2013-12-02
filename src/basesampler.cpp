#include "basesampler.h"

BaseSampler::BaseSampler(QObject *parent) : QObject(parent){
    mSample = new QByteArray();
}

BaseSampler::~BaseSampler(){
    delete mSample;
}
