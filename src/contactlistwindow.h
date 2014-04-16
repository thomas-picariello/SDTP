#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QList>
#include <QListWidgetItem>
#include <QTcpServer>
#include <QPixmap>
#include <QPainter>
#include "editcontactwindow.h"
#include "settingswindow.h"
#include "contact.h"
#include "networkmanager.h"
#include "contactitemwidget.h"

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum Status{
        Offline,
        Online,
        Busy,
        Away
    };
    enum ItemDataRole{
        IdRole = 33,
        StatusRole = 34
    };

    explicit ContactListWindow(ContactDB *contactDB,
                               QPair<QByteArray,QByteArray> *fileKey,
                               QWidget *parent = 0);
    ~ContactListWindow();

    void setContactStatusIcon(QListWidgetItem *item, Status status);
    void setContactStatusIcon(int id, Status status);
    void setContactStatusIcon(Contact *contact, Status status);

public slots:
    void acceptConnection();
    void addContact();
    void connectToContact();
    void editContact();
    void listSelectionChanged(QListWidgetItem *current,QListWidgetItem *previous);
    void listItemClicked(QListWidgetItem *item);
    void openSettingsWindow();
    void onListItemAction(int id, ContactItemWidget::Action action);
    void refreshList();
    void deleteContact();
    void restartListener();
    
private:
    ContactDB *mContactDB;
    QPair<QByteArray,QByteArray> *mFileKey;
    Ui::ContactListWindow *ui;
    QTcpServer *mListener;
    QList<NetworkManager*> mManagerList;

    Contact* getSelectedContact();
    QListWidgetItem* findItemByContactId(int id);
};

#endif // CONTACTLISTWINDOW_H
