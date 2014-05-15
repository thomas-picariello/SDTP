#ifndef ROOTAPP_H
#define ROOTAPP_H

#include <QObject>
#include "abstractapp.h"

class RootApp : public AbstractApp
{
    Q_OBJECT
public:
    explicit RootApp(QWidget *parent = 0);

    int getAppID();

public slots :
    virtual void dataToRead(QByteArray data);


private :


};

#endif // ROOTAPP_H
