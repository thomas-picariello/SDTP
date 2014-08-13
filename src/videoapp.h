#ifndef VIDEOAPP_H
#define VIDEOAPP_H

#include <contact.h>
//#include <QDebug>
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

/**
 * @brief VideoApp is an app specifically designed to acquire and display Video Streams.
 *
 */
class VideoApp : public AbstractApp
{
    Q_OBJECT

public:
    static const AppType APPTYPE = VideoStreamer;

    explicit VideoApp(Contact *contact, QWidget *parent = 0);
    virtual ~VideoApp();

public slots :
    virtual void readIncommingData(const QByteArray &data);
    void onDataToSend(QByteArray data);
signals :
    void error();
protected :
    virtual void closeevent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);


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
