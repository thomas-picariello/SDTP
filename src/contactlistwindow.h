#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QList>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "starter.h"
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
    //Starter *mStarter;
    QList<NetworkManager*> m_ManagerList;
};

#endif // CONTACTLISTWINDOW_H
