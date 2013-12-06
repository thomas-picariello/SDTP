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
    void onAddBtClick();
    void onEditBtClick();
    void onRemoveBtClick();
    void onConnectBtClick();
    void onSettingsBtClick();
    void onExitBtClick();
    void refreshList();
    
private:
    Ui::ContactListWindow *ui;
    EditContactWindow *mEditContactWindow;
    SettingsWindow *mSettingsWindow;
    Starter *mStarter;
};

#endif // CONTACTLISTWINDOW_H
