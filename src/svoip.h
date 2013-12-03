#ifndef SVOIP_H
#define SVOIP_H

#include "mainwindow.h"

class SVoIP
{
public:
    SVoIP();
    void GetListener();
    ~SVoIP();

private:
    MainWindow *window;

};

#endif // SVOIP_H
