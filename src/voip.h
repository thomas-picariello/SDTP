#ifndef VOIP_H
#define VOIP_H

#include <QObject>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QTimer>
#include <QDebug>
#include "contact.h"

class VoIP : public QObject
{
    Q_OBJECT
public:
    explicit VoIP(QObject *parent = 0);
    ~VoIP();
    void call(Contact contact);
    void endCall();
    
signals:
    void incommingCall();
    
public slots:

private:
    QAudioInput *mAudioInput;
};

#endif // VOIP_H
