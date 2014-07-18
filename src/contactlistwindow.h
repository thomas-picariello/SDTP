#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QCloseEvent>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "contact.h"
#include "contactactionswidget.h"
#include "abstractapp.h"
#include "applauncheritem.h"
#include "typesenums.h"

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum ItemDataRole{
        IdRole = 33
    };

    explicit ContactListWindow(ContactDB *contactDB,
                               RsaKeyring *keyring,
                               QPair<QByteArray,QByteArray> *fileKey,
                               QWidget *parent = 0);
    ~ContactListWindow();



signals:
    void startApp(Contact* contact, AppType appTypeId);
//    void contactAction(int contactId, ContactDB::Action action);
    void settingsUpdated();

public slots:


private slots:
    void addContact();
    void listItemClicked(QListWidgetItem *currentItem);
    void openSettingsWindow();
    void onContactAction(int contactId, ContactDB::Action action);
    void onStartApp(int contactId, AppType type);
    void refreshList();
    void updateContactStatus();

private:
    Ui::ContactListWindow *ui;
    ContactDB *mContactDB;
    SettingsWindow mSettingsWindow;
    EditContactWindow mEditContactWindow;

    virtual void closeEvent(QCloseEvent* event);
    Contact* getSelectedContact();
    QListWidgetItem* findItemByContactId(int id);
    void setContactStatusIcon(QListWidgetItem *item, Contact::Status status);
};

#endif // CONTACTLISTWINDOW_H
