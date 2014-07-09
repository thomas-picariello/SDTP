#include "videoapp.h"
#include "ui_videoapp.h"


VideoApp::VideoApp(Contact * contact, QWidget* parent) :
    AbstractApp(contact, parent),
    ui(new Ui::VideoApp)
{
    ui->setupUi(this);
    show();


    updateDisplay();

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
    delete ui;
}
