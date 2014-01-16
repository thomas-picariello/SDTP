#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QList>
#include <QTcpServer>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "contact.h"
#include "networkmanager.h"

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit ContactListWindow(QWidget *parent = 0);
    ~ContactListWindow();

public slots:
    void acceptConnection();
    void addContact();
    void editContact();
    void removeContact();
    void connectToContact();
    void openSettingsWindow();
    void exitApp();
    void refreshList();
    
private:
    Ui::ContactListWindow *ui;
    EditContactWindow *mEditContactWindow;
    SettingsWindow *mSettingsWindow;
    QTcpServer *mListener;

    QList<Contact> mContactList;
    QList<NetworkManager*> m_ManagerList;
};

#endif // CONTACTLISTWINDOW_H
