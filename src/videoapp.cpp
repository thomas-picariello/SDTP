#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp)
{


    wrapper = new ViewFinderWrapper();




    wrapper->setWidth(800);
    wrapper->setHeight(600);


    wrapper->startCamera();

    connect(wrapper,SIGNAL(newFrameAvaillable(QVideoFrame)),this,SLOT(drawFrame(QVideoFrame)));



    ui->setupUi(this);




}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}


void VideoApp::readIncommingData(QByteArray &data){

}
void VideoApp::drawFrame(QVideoFrame frame)
{
    frame.map(QAbstractVideoBuffer::ReadOnly);



    QImage img(frame.bits(),
    frame.width(),
    frame.height(),
    QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));


    qDebug()<<frame<<"  "<<img<<"  ";
    ui->label->setPixmap(QPixmap::fromImage(img));

    //update();
    frame.unmap();


}

void VideoApp::senddata(){

}
VideoApp::~VideoApp(){

    delete ui;
}
