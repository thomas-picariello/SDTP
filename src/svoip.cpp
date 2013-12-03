#include "svoip.h"

SVoIP::SVoIP()
{
    window = new MainWindow();
}
void SVoIP::GetListener()
{
    //Listener listener;
}
SVoIP::~SVoIP()
{
    delete window;
}
