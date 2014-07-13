#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "contact.h"
#include "contactactionswidget.h"
#include "abstractapp.h"
#include "typesenums.h"

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum ItemDataRole{
        IdRole = 33,
        StatusRole = 34
    };

    explicit ContactListWindow(ContactDB *contactDB,
                               RsaKeyring *keyring,
                               QPair<QByteArray,QByteArray> *fileKey,
                               QWidget *parent = 0);
    ~ContactListWindow();

    void setContactStatusIcon(QListWidgetItem *item, Contact::Status status);
    void setContactStatusIcon(int id, Contact::Status status);

signals:
    void startApp(Contact* contact, AppType appTypeId);
    void contactEvent(int contactId, Contact::Event);
    void settingsUpdated();

public slots:
    void addContact();
    void editContact();
    void listItemClicked(QListWidgetItem *currentItem);
    void openSettingsWindow();
    void onListItemAction(int contactId, ContactActionsWidget::Action action);
    void refreshList();
    void deleteContact();

private slots:
    void updateContactStatusIcon();

private:
    Ui::ContactListWindow *ui;
    ContactDB *mContactDB;
    SettingsWindow mSettingsWindow;
    EditContactWindow mEditContactWindow;

    Contact* getSelectedContact();
    QListWidgetItem* findItemByContactId(int id);
};

#endif // CONTACTLISTWINDOW_H
