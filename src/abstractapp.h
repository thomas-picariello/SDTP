#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>
#include <QHash>

#include "typesenums.h"
#include "contact.h"
#include "contactdb.h"

class AbstractApp : public QWidget
{
    Q_OBJECT
public:
    enum Error{
        IncompatibleVersion //TODO: add more errors
    };

    explicit AbstractApp(Contact* contact, QWidget *parent=0): QWidget(parent){ m_ContactList.append(contact); }

    QList<Contact*> getContactList(){ return m_ContactList; }
    void addContact(Contact* contact){ m_ContactList.append(contact); }
    void removeContact(Contact* contact){ m_ContactList.removeOne(contact); }

public slots :
    virtual void readIncommingData(QByteArray &data) = 0;

signals :
    void sendData(LinkType linkType, QByteArray &data);
    void requestStartApp(AppType type, int contactID);
    void error(AbstractApp::Error);

protected:
    QList<Contact*> m_ContactList;
};



#endif // ABSTRACTAPP_H
