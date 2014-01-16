#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include "contactlistwindow.h"

class SVoIP : public QObject
{

    Q_OBJECT

public:
    SVoIP(QObject *parent = 0);
    ~SVoIP();


private:
    ContactListWindow *mWindow;
};

#endif // SVOIP_H
