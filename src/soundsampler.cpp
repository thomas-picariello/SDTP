#include "soundsampler.h"

SoundSampler::SoundSampler(){
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(8000);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }

    mAudioInput = new QAudioInput(format);
    mBuffer = new QBuffer();

    connect(mAudioInput, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(handleStateChanged(QAudio::State)));
}

void SoundSampler::stopRecording(){
    mAudioInput->stop();
    mBuffer->close();
    mSample->clear();

    //init the PRNG
    qsrand(mBuffer->data().at(mBuffer->data().length()/2));
    for(int i=0; i<4; i++){
        //generate a random index
        int index = qrand() % mBuffer->data().length();
        mSample->append(mBuffer->data().at(index));
    }
    qDebug() << "Seed from sound sampler: " << mSample->toHex();
    emit sampleAvailable(mSample);
}

void SoundSampler::handleStateChanged(QAudio::State newState){
    switch (newState) {
        case QAudio::StoppedState:
            if (mAudioInput->error() != QAudio::NoError) {
                qWarning() << mAudioInput->error();
            } else {
                qDebug() << "Finished recording audio sample";
            }
            break;

        case QAudio::ActiveState:
            qDebug() << "Starting recording audio sample";
            break;

        default:
            qDebug() << "AudioInput entering state " << newState;
            break;
    }
}

void SoundSampler::getSample(){
    QTimer::singleShot(1, this, SLOT(stopRecording()));
    mBuffer->open(QBuffer::ReadWrite);
    mAudioInput->start(mBuffer);
}

SoundSampler::~SoundSampler(){
    delete mAudioInput, mBuffer;
}
