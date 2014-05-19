#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>
#include "apptypeidenum.h"

class AbstractApp : public QWidget
{
    Q_OBJECT
public:
    struct AppUID{
        AppTypeID appTypeID;
        uint appInstanceID;
        AppUID(): appTypeID(Root), appInstanceID(0){}
        AppUID(AppTypeID typeId, uint instanceId = 0): appTypeID(typeId), appInstanceID(instanceId){}
        bool operator <(const AppUID &second) const{
            if(appTypeID == second.appTypeID)
                return (appInstanceID < second.appInstanceID);
            return (appTypeID < second.appTypeID);
        }
    };

public slots :
    virtual void dataToRead(QByteArray data) = 0;

signals :
    void dataToSend(QByteArray);

};

#endif // ABSTRACTAPP_H
