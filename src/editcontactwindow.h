#ifndef EDITCONTACTWINDOW_H
#define EDITCONTACTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QString>
#include <QIntValidator>
#include <QRegExpValidator>
#include "contact.h"
#include "contactdb.h"

namespace Ui {
class EditContactWindow;
}

class EditContactWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditContactWindow(ContactDB *contactDB,
                               QWidget *parent = 0);
    ~EditContactWindow();
    void open(Contact* contact);
    void clear();

signals:
    void contactEvent(int, Contact::Event);

public slots:
    void addHost();
    void cancel();
    void removeHost();
    void save();
    
protected:
    void hideEvent(QHideEvent* hideEvent);

private:
    ContactDB *mContactDB;
    Ui::EditContactWindow *ui;
    Contact *mContact;
    QIntValidator mPortValidator;
};

#endif // EDITCONTACTWINDOW_H
