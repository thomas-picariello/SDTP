#ifndef VOIP_H
#define VOIP_H

#include <QObject>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QTimer>
#include <QDebug>
#include "contact.h"
#include "qopusdevice.h"

class VoIP : public QObject
{
    Q_OBJECT
public:
    explicit VoIP(QObject *parent = 0);
    ~VoIP();
    void call(Contact contact);
    void endCall();
    QOpusDevice* getOpusIODevice();
    
signals:
    void incommingCall();
    
public slots:
    void takeIncommingCall(QIODevice* dataInterface);

private:
    QAudioInput *mAudioInput;
    QAudioOutput *mAudioOutput;
    QOpusDevice *mOpusIODevice;
};

#endif // VOIP_H
