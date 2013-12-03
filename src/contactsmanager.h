#ifndef CONTACTSMANAGER_H
#define CONTACTSMANAGER_H


#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QLayout>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include "contact.h"


class ContactsManager: public QWidget
{

    Q_OBJECT

public:
    ContactsManager();
    ~ContactsManager();

public slots:
     void contactsWindow();
     void addcontact();
     void editcontact();
     void savecontact();

private:
    QWidget *Contactwindow, *addcontactwindow, *editcontactwindow;
    QPushButton *About, *addContact,*editContact, *saveContact;
    QSettings *settings;
    QVBoxLayout *vlayout, *addclayout, *addlabellayout, *adddatalayout;
    QHBoxLayout *hlayout, *addchlayout;
    QLabel *label, *addlabel, *IPlabel, *portlabel, *namelabel, *keylabel;
    Contact *contact;
    QMenu *menu;
    QValidator *IPvali;
    QLineEdit *IP, *port, *name, *key;


};

#endif // CONTACTSMANAGER_H
