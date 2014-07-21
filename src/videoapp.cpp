#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp){

    m_Canvas  = new QGLCanvas();
    m_Canvas2 = new QGLCanvas();

    m_camera  = new QCamera(this);
    m_surface = new VideoSurface();
    thread = new QThread();
    m_videoprocessor = new VideoProcessor();

    m_videoprocessor->moveToThread(thread);




    QList<QByteArray> cameras = m_camera->availableDevices();
    m_camera = new QCamera(cameras.first());
    m_camera->setViewfinder(new QCameraViewfinder);

    if (m_camera) {
        m_camera->start();
        if(m_camera->state()==QCamera::ActiveState)m_camera->setViewfinder(m_surface);
    }


    connect(m_surface,SIGNAL(newFrame(QVideoFrame)),m_videoprocessor,SLOT(process(QVideoFrame)));
    connect(m_videoprocessor, SIGNAL(finished(QByteArray)), this, SLOT(onDataToSend(QByteArray)));

    connect(m_videoprocessor, &VideoProcessor::imgForDisplay,m_Canvas,&QGLCanvas::setImage);
    connect(m_videoprocessor, &VideoProcessor::decoded,m_Canvas2,&QGLCanvas::setImage);

    ui->setupUi(this);




    layout()->addWidget(m_Canvas);
    layout()->addWidget(m_Canvas2);

    this->adjustSize();

    thread->start();


}
void VideoApp::addContact(){

}
void VideoApp::updateDisplay(){

}
void VideoApp::readIncommingData(const QByteArray &data){
    m_videoprocessor->decode(data);
}

void VideoApp::onDataToSend(QByteArray data)
{
    //readIncommingData(data);
    emit sendData(LinkType::TCP,data);
}
VideoApp::~VideoApp(){
    delete m_Canvas,m_Canvas2,m_camera,m_surface,m_videoprocessor;
    delete ui;
}
