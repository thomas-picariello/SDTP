#include "starterManager.h"

Starter::Starter()
{
    starterwindow = new QWidget;
    about = new QPushButton("AboutQt",starterwindow);

    connect(about,SIGNAL(clicked()),qApp,SLOT(aboutQt()));

}
void Starter::starterWindow()
{
    qDebug()<<"Test for starterWindow";
    starterwindow->show();
}
Starter::~Starter()
{

}
