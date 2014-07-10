#ifndef VIDEOAPP_H
#define VIDEOAPP_H

#include <contact.h>
#include <QList>
#include <QScrollBar>
#include <QtMultimedia>
#include <QVideoWindowControl>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlayer>
#include <QMediaObject>
#include <QCamera>
#include <QDebug>
#include "typesenums.h"
#include "abstractapp.h"

namespace Ui{
class VideoApp;

}

class VideoApp : public AbstractApp
{
    Q_OBJECT

public:
    static const AppType APPTYPE = VideoStreamer;

    explicit VideoApp(Contact *contact, QWidget *parent = 0);
    virtual ~VideoApp();

    void addContact();
    void updateDisplay();

public slots :
    virtual void readIncommingData(QByteArray &data);

private slots:
    void senddata();

private:
    Ui::VideoApp *ui;
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QCamera *m_Camera;

};

#endif // VIDEOAPP_H
