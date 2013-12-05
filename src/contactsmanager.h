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
    QWidget *m_contactManagerMainwindow, *m_addContactWindow, *m_editContactWindow;
    QPushButton *m_addContactButton,*m_editContactButton, *m_saveContactButton;
    QSettings *m_settings;
    QVBoxLayout *m_addAndEditButtonLayout, *m_addWindowMainLayout, *m_addWindowLabelLayout, *m_addWindowLineEditLayout;
    QHBoxLayout *m_contactManagerWindowMainLayout, *m_addWindowCenterLayout;
    QLabel *m_contactListLabel, *m_addlabel, *m_IPlabel, *m_portlabel, *m_namelabel, *m_keylabel;
    Contact *m_contact;
    QMenu *menu;
    QValidator *IPvali;
    QLineEdit *IP, *port, *name, *key;


};

#endif // CONTACTSMANAGER_H
