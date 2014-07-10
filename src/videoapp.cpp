#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp)
{

    QList<QByteArray> cameras = QCamera::availableDevices();

    if(!cameras.isEmpty()){
        m_Camera = new QCamera(cameras.first());


        m_videoWidget = new QVideoWidget();
        m_Camera->setViewfinder(m_videoWidget);
        m_Camera->start();

        m_videoWidget->show();



    }else{
        m_player = new QMediaPlayer();

        m_player->setMedia(QUrl::fromLocalFile("test.avi"));

        m_videoWidget = new QVideoWidget();

        m_player->setVideoOutput(m_videoWidget);

        m_videoWidget->show();

        m_player->play();
    }


    ui->setupUi(this);


}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}
void VideoApp::readIncommingData(QByteArray &data){

}
void VideoApp::senddata(){

}
VideoApp::~VideoApp(){
    m_Camera->stop();
    delete ui;
}
