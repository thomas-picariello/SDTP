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
#include "contactitemwidget.h"
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

public slots:
    void addContact();
    void editContact();
    void listItemClicked(QListWidgetItem *currentItem);
    void openSettingsWindow();
    void onListItemAction(int contactId, ContactItemWidget::Action action);
    void refreshList();
    void deleteContact();

signals:
    void startApp(QList<Contact*>, AppType appTypeId);
    void contactEvent(int contactId, Contact::Event);
    void settingsUpdated();
    
private:
    Ui::ContactListWindow *ui;
    ContactDB *mContactDB;
    SettingsWindow mSettingsWindow;
    EditContactWindow mEditContactWindow;

    Contact* getSelectedContact();
    QListWidgetItem* findItemByContactId(int id);
};

#endif // CONTACTLISTWINDOW_H
