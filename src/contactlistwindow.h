#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include "editcontactwindow.h"
#include "starter.h"

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
    void onExitBtClick();
    void refreshList();
    
private:
    Ui::ContactListWindow *ui;
    EditContactWindow *mEditContactWindow;
    Starter *mStarter;
};

#endif // CONTACTLISTWINDOW_H
