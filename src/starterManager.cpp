#include "startermanager.h"

StarterManager::StarterManager()
{
    starterwindow = new QWidget;
    about = new QPushButton("AboutQt",starterwindow);

    connect(about,SIGNAL(clicked()),qApp,SLOT(aboutQt()));

}
void StarterManager::starterWindow()
{
    qDebug()<<"Test for starterWindow";
    starterwindow->show();
}
StarterManager::~StarterManager()
{

}
