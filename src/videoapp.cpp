#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp)
{


    wrapper = new ViewFinderWrapper();
    widget = new QVideoWidget;

    wrapper->setWidth(800);
    wrapper->setHeight(600);


    wrapper->startCamera();

    connect(wrapper,SIGNAL(newFrameAvaillable(QImage)),this,SLOT(drawFrame(QImage)));



    ui->setupUi(this);
    widget->show();

}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}


void VideoApp::readIncommingData(QByteArray &data){

}

void VideoApp::drawFrame(QImage frame)
{



   // qDebug()<<frame.scanLine(1);




}
void VideoApp::senddata(){

}
VideoApp::~VideoApp(){

    delete ui;
}
