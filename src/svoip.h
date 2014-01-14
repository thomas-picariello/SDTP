#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include "contactlistwindow.h"
#include "networkmanager.h"

class SVoIP : public QObject
{

    Q_OBJECT

public:
    SVoIP(QObject *parent = 0);
    ~SVoIP();

public slots :
    void acceptConnection();

private:
    ContactListWindow *mWindow;
    QTcpServer *mListener;
    QList<NetworkManager*> m_ManagerList;
};

#endif // SVOIP_H
