#ifndef EDITCONTACTWINDOW_H
#define EDITCONTACTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QString>
#include <QIntValidator>
#include <QRegExpValidator>
#include "contact.h"

namespace Ui {
class EditContactWindow;
}

class EditContactWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditContactWindow(Contact *contact, QWidget *parent = 0);
    ~EditContactWindow();
    void fill(QString contactName);
    void clear();

signals:
    void contactChanged();

public slots:
    void save();
    void cancel();
    void saveAfterResolving();
    
private:
    Ui::EditContactWindow *ui;
    Contact *mContact;
    QIntValidator mPortValidator;
    QRegExpValidator mIpValidator;

    bool isIpAddress(QString host);

};

#endif // EDITCONTACTWINDOW_H
