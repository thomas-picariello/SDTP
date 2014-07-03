#include "appmanager.h"

AppManager::AppManager(QObject* parent):
    QObject(parent)
{

}

void AppManager::readIncommingData(QByteArray &data){

}

void AppManager::startApp(AppType appType){

}

void AppManager::requestPartnerApp(AppType){

}

void AppManager::registerApp(AppUID localAppUID, AbstractApp* app){

}

void AppManager::unregisterApp(AppUID localAppUID){

}

AppManager::~AppManager(){

}
