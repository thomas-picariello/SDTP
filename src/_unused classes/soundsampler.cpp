#include "soundsampler.h"

SoundSampler::SoundSampler(){
    QAudioFormat format;
        format.setChannelCount(1);
        format.setSampleRate(44100);
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
    //TODO: gestion des erreurs (crash quand buffer vide)
    //init the PRNG
    qsrand(mBuffer->data().at(mBuffer->data().length()/2));
    for(int i=0; i<4; i++){
        //generate a random index
        int index = qrand() % mBuffer->data().length();
        mSample->append(mBuffer->data().at(index));
    }
    //mSample->append(mBuffer->data());
    emit sampleAvailable(mSample);
}

void SoundSampler::handleStateChanged(QAudio::State newState){
    qDebug() << newState;
}

void SoundSampler::getSample(){
    mBuffer->open(QBuffer::ReadWrite | QBuffer::Truncate);
    mAudioInput->start(mBuffer);
    QTimer::singleShot(100, this, SLOT(stopRecording()));
}

SoundSampler::~SoundSampler(){
    delete mAudioInput, mBuffer;
}
