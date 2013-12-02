#include "settingsManager.h"
#include <QDebug>

Settings::Settings()
{
    setwindow = new QWidget();
    about = new QPushButton("AboutQt", setwindow);

    connect(about,SIGNAL(clicked()),qApp,SLOT(aboutQt()));

}
void Settings::settingsWindow()
{

    qDebug()<<"Test for SettingsWindow !";
    setwindow->show();

}
Settings::~Settings()
{

}
