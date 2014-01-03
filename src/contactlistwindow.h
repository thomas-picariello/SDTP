#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "starter.h"
#include "contact.h"

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
    Starter *mStarter;
};

#endif // CONTACTLISTWINDOW_H
