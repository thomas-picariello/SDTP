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



    connect(wrapper,SIGNAL(newFrameAvaillable(QImage*)),
            this,SLOT(drawFrame(QImage*)));
    connect(wrapper,SIGNAL(newFrameToSend(QByteArray)),this,SLOT(onDataToSend(QByteArray)));

    ui->setupUi(this);

    layout()->addWidget(m_Canvas);



}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}
void VideoApp::readIncommingData(const QByteArray &data){

    qDebug()<<"data Incoming";

}
void VideoApp::drawFrame(QImage *frame){

    m_Canvas->setImage(*frame);

}

void VideoApp::onDataToSend(QByteArray data)
{

   // emit sendData(LinkType::TCP,data);


}
VideoApp::~VideoApp(){

    delete ui;
}
