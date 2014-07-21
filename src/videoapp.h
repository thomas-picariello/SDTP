#ifndef VIDEOAPP_H
#define VIDEOAPP_H



#include <contact.h>
#include <QDebug>
#include <QCamera>
#include <QCameraViewfinder>
#include <QImageEncoderSettings>
#include <QThread>

#include "typesenums.h"
#include "abstractapp.h"

#include "qglcanvas.h"
#include "videosurface.h"
#include "videoprocessor.h"



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
    virtual void readIncommingData(const QByteArray &data);
    void onDataToSend(QByteArray data);

signals :
    void error();

private:
    Ui::VideoApp *ui;
    QGLCanvas *m_Canvas;
    QGLCanvas *m_Canvas2;
    QCamera* m_camera;
    VideoSurface *m_surface;
    QThread *thread;
    VideoProcessor *m_videoprocessor;
};

#endif // VIDEOAPP_H
