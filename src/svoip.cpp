#include "svoip.h"

SVoIP::SVoIP(QObject *parent) : QObject(parent){

    mWindow = new ContactListWindow();

}

SVoIP::~SVoIP(){
    delete mWindow;
}
