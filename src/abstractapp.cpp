#include "abstractapp.h"

AbstractApp::AbstractApp( QWidget *parent):
    QWidget(parent)
{

    mAppID = NULL;


}

void AbstractApp::dataToRead(QByteArray data){



}

AbstractApp::~AbstractApp(){


}
int AbstractApp::getAppID(){
    return mAppID;
}
