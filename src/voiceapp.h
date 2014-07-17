#ifndef VOICEAPP_H
#define VOICEAPP_H

#include <QLabel>
#include <QTimer>

#include "abstractapp.h"

namespace Ui{
class VoiceApp;
}

class VoiceApp : public AbstractApp
{
    Q_OBJECT
public:
    static const int MONITORS_UPDATE_INTERVAL = 200;

    enum State{
        Disconnected,
        Ready,
        Calling
    };

    explicit VoiceApp(Contact* contact, QWidget *parent = 0);
    ~VoiceApp();

signals:

public slots:
    virtual void readIncommingData(QByteArray &data);

private slots:
    void endCall();
    void onContactStatusChange();
    void startCall();
    void updateMonitorsValue();

private:
    Ui::VoiceApp *ui;
    QTimer m_monitorUpdateTimer;
    State m_state;

    void onStateChange();
};

#endif // VOICEAPP_H
