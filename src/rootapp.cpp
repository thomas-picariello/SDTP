#include "rootapp.h"

RootApp::RootApp(QWidget *parent) :
    AbstractApp(parent)
{

    mAppID = 0;
}
void RootApp::dataToRead(QByteArray data){

}

int RootApp::getAppID(){
    return mAppID;
}
