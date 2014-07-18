#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp){


    wrapper = new ViewFinderWrapper();
    wrapper->setWidth(800);
    wrapper->setHeight(600);
    wrapper->startCamera();

    m_Canvas = new QGLCanvas();



    connect(wrapper,SIGNAL(newFrameAvaillable(QVideoFrame)),this,SLOT(drawFrame(QVideoFrame)));

    ui->setupUi(this);

    layout()->addWidget(m_Canvas);

}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}
void VideoApp::readIncommingData(QByteArray &data){


}
void VideoApp::drawFrame(QVideoFrame frame){
    frame.map(QAbstractVideoBuffer::ReadOnly);

//    ui->label->setPixmap(QPixmap::fromImage(QImage(frame.bits(),
//                                                   frame.width(),
//                                                   frame.height(),
//                                                   QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()))));


    m_Canvas->setImage(QImage(frame.bits(),
                             frame.width(),
                             frame.height(),
                             QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat())).copy(QRect(0,0,frame.width(),frame.height())));

    frame.unmap();

}
VideoApp::~VideoApp(){

    delete ui;
}
