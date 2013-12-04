#ifndef SVOIP_H
#define SVOIP_H

#include <QSettings>
#include "mainwindow.h"
#include "listener.h"

class SVoIP
{
public:
    SVoIP();
    ~SVoIP();

private:
    MainWindow *mWindow;
    Listener *mListener;
};

#endif // SVOIP_H
