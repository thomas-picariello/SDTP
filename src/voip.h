#ifndef VOIP_H
#define VOIP_H

#include <QDebug>
#include <QObject>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include "contact.h"
#include "qopusdevice.h"
#include "qjrtp.h"

class VoIP : public QObject
{
    Q_OBJECT
public:
    enum CallState{ONLINE, OFFLINE};
    enum Error{UNDERLYING_DEVICE_CLOSED};

    explicit VoIP(QObject *parent = 0);
    explicit VoIP(QIODevice *interfaceIODevice,
                  QObject *parent = 0);
    void call();
    void endCall();
    QOpusDevice* getOpusDevice();
    CallState getCallState();
    ~VoIP();

signals:
    void callStateChanged(CallState state);
    void error(Error err);
    
public slots:
    void takeIncommingCall();
    void startAudioOutput();
    //Debug
    void inputStateChanged(QAudio::State state);
    void outputStateChanged(QAudio::State state);

private:
    QAudioInput *mAudioInput;
    QAudioOutput *mAudioOutput;
    QOpusDevice *mOpus;
    QJrtp *mJrtp;
    QIODevice *mDataInterface;
    CallState mCallState;

    void initAudio();
};

#endif // VOIP_H
