#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QList>
#include <QListWidgetItem>
#include <QTcpServer>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "contactfactory.h"
#include "contact.h"
#include "networkmanager.h"

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum ItemDataRole{
        IdRole = 0x0101
    };

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
    void restartListener();
    
private:
    Ui::ContactListWindow *ui;
    QTcpServer *mListener;
    QList<QListWidgetItem*> mItemList;
    QList<Contact*> mContactList;
    QList<NetworkManager*> mManagerList;

    Contact* getSelectedContact();
};

#endif // CONTACTLISTWINDOW_H
