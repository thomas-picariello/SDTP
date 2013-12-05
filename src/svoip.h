#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include "contactlistwindow.h"
#include "responder.h"

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
    QList<Responder*> m_responderList;
};

#endif // SVOIP_H
