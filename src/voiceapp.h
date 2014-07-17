#ifndef VOICEAPP_H
#define VOICEAPP_H

#include <QLabel>

#include "abstractapp.h"

namespace Ui{
class VoiceApp;
}

class VoiceApp : public AbstractApp
{
    Q_OBJECT
public:
    explicit VoiceApp(Contact* contact, QWidget *parent = 0);
    ~VoiceApp();

signals:

public slots:
    virtual void readIncommingData(QByteArray &data);

private slots:
    void updateContactNames();

private:
    Ui::VoiceApp *ui;
};

#endif // VOICEAPP_H
