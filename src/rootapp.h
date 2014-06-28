#ifndef ROOTAPP_H
#define ROOTAPP_H

#include "abstractapp.h"
#include "typesenums.h"
#include "contact.h"
#include "contactdb.h"

class RootApp : public AbstractApp
{
    Q_OBJECT
public:
    static const AppType APPTYPE = Root;

    explicit RootApp(QList<Contact*> contactList, QWidget *parent = 0);
    ~RootApp();

public slots:
    void readIncommingData(QByteArray &data);

};

#endif // ROOTAPP_H
