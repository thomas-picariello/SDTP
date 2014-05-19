#ifndef ROOTAPP_H
#define ROOTAPP_H

#include <QObject>
#include "abstractapp.h"

class RootApp : public AbstractApp
{
    Q_OBJECT
public:
    static const uint APPID = 0;

    explicit RootApp(QWidget *parent = 0);

    int getAppID();

public slots :
    virtual void dataToRead(QByteArray data);

};

#endif // ROOTAPP_H
