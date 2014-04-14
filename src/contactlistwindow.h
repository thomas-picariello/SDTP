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

namespace Ui {
class ContactListWindow;
}

class ContactListWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum ItemDataRole{
        IdRole = 0x0101,
        StatusRole = 0x102
    };
    enum ContactStatus{
        Offline,
        Online,
        Busy,
        Away
    };

    explicit ContactListWindow(ContactDB *contactDB,
                               QPair<QByteArray,QByteArray> *fileKey,
                               QWidget *parent = 0);
    ~ContactListWindow();

    void setContactStatusIcon(QListWidgetItem *item, ContactStatus status);
    void setContactStatusIcon(int id, ContactStatus status);
    void setContactStatusIcon(Contact *contact, ContactStatus status);

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
    ContactDB *mContactDB;
    QPair<QByteArray,QByteArray> *mFileKey;
    Ui::ContactListWindow *ui;
    QTcpServer *mListener;
    QList<NetworkManager*> mManagerList;

    Contact* getSelectedContact();
    QListWidgetItem* findItemByContactId(int id);
};

#endif // CONTACTLISTWINDOW_H
