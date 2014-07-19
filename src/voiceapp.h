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

    explicit VoiceApp(Contact* contact, QWidget *parent = 0);
    ~VoiceApp();

signals:

public slots:
    virtual void readIncommingData(const QByteArray& data);

private slots:
    void endCall();
    void onContactStatusChange();
    void onCodecReadyRead();
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
    void updateUiToState();
};

#endif // VOICEAPP_H
