#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp){

    m_Canvas = new QGLCanvas();
    m_Canvas2 = new QGLCanvas();

    m_camera = new QCamera(this);

    m_surface = new VideoSurface();

   m_encoder = new VideoEncoder;

    QList<QByteArray> cameras = m_camera->availableDevices();

    m_camera = new QCamera(cameras.first());
    m_camera->setViewfinder(new QCameraViewfinder);

    if (m_camera) {

        m_camera->start();
        if(m_camera->state()==QCamera::ActiveState)m_camera->setViewfinder(m_surface);

    }

    ui->setupUi(this);

    layout()->addWidget(m_Canvas);
    layout()->addWidget(m_Canvas2);



    connect(m_surface,SIGNAL(newFrame(QImage)),this,SLOT(drawFrame(QImage)));
    connect(m_encoder,SIGNAL(frameProcessed(QByteArray)),this,SLOT(onDataToSend(QByteArray)));

    m_encoder->start();
}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}
void VideoApp::readIncommingData(const QByteArray &data){


    //m_Canvas2->setImage(&QImage::fromData(data,"JPG"));
    qDebug()<<"data Incoming";

}
void VideoApp::drawFrame(QImage frame){


    m_Canvas->setImage(&frame);
    m_encoder->addFrameToProcessingQueue(&frame);


}
void VideoApp::onDataToSend(QByteArray data)
{
    qDebug()<<"data : "<<data.size()<<"bytes";
    m_Canvas2->setImage(&QImage::fromData(data,"JPG"));
    emit sendData(LinkType::TCP,data);


}
VideoApp::~VideoApp(){

    delete ui;
}
