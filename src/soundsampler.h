#ifndef SOUNDSAMPLER_H
#define SOUNDSAMPLER_H

#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QTimer>
#include <QDebug>
#include "basesampler.h"

class SoundSampler : public BaseSampler
{
    Q_OBJECT
public:
    SoundSampler();
    ~SoundSampler();

public slots:
    void getSample();
    void handleStateChanged(QAudio::State newState);
    void stopRecording();

private:
    QAudioInput *mAudioInput;
    QBuffer *mBuffer;
};

#endif // SOUNDSAMPLER_H
