#ifndef VIDEOAPP_H
#define VIDEOAPP_H



#include <contact.h>
#include <QDebug>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDeclarativeContext>
#include <QtGui>

#include "typesenums.h"
#include "abstractapp.h"
#include "viewfinderwrapper.h"


// VP8 Code
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#include "vpx/vp8.h"

#define interface (vpx_codec_vp8_cx())
#define fourcc    0x30385056

#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)

////////

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
    void drawFrame(QImage frame);

signals :
    void error();

private slots:
    void senddata();

private:
    Ui::VideoApp *ui;
    ViewFinderWrapper *wrapper;
    QVideoWidget *widget;







};

#endif // VIDEOAPP_H
