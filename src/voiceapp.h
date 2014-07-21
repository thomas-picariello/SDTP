#ifndef VOICEAPP_H
#define VOICEAPP_H

#include <QLabel>
#include <QTimer>

#include "opusvoicecodec.h"
#include "abstractapp.h"

namespace Ui{
class VoiceApp;
}

class VoiceApp : public AbstractApp
{
    Q_OBJECT
public:
    static const AppType APPTYPE = VoIP;

    enum State{
        Disconnected,
        Ready,
        Calling
    };

    enum Controls:quint8{
        StartCall = 0x00,
        EndCall = 0x01
    };

    struct Packet{
        enum Type:quint8{
            Control = 0x00,
            Data = 0x01
        } type;
        QByteArray payload;
        Packet(){}
        Packet(Type _type, QByteArray _payload): type(_type), payload(_payload){}
    };

    explicit VoiceApp(Contact* contact, QWidget *parent = 0);
    ~VoiceApp();

signals:

public slots:
    virtual void readIncommingData(const QByteArray& data);

private slots:
    void endCall();
    void onContactStatusChange();
    void onCodecReadyRead();
    void onCallButtonClick();
    void startCall();
    void onNewInputProbe(qint16 probe);
    void onNewOutputProbe(qint16 probe);
    void setOutputVolume(int value);
    void setInputMute(bool mute);
    void setOutputMute(bool mute);

private:
    Ui::VoiceApp* m_ui;
    OpusVoiceCodec m_codec;
    State m_state;

    virtual void closeEvent(QCloseEvent* event);
    void sendPacket(Packet::Type type, const QByteArray& payload);
    void updateUiToState();
};

QDataStream& operator <<(QDataStream &out, const VoiceApp::Packet &packet);
QDataStream& operator >>(QDataStream &in, VoiceApp::Packet &packet);

#endif // VOICEAPP_H
