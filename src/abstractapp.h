#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QObject>

class AbstractApp
{
public:
    AbstractApp();

public slots :
    void newDataEvent(QByteArray data);

};

#endif // ABSTRACTAPP_H
