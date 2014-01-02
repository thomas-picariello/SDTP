#ifndef EDITCONTACTWINDOW_H
#define EDITCONTACTWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QString>
#include "contact.h"

namespace Ui {
class EditContactWindow;
}

class EditContactWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditContactWindow(QWidget *parent = 0);
    explicit EditContactWindow(QString name, QWidget *parent = 0);
    ~EditContactWindow();
    void fill(QString contactName);
    void clear();

signals:
    void contactChanged();

public slots:
    void save();
    void cancel();
    
private:
    Ui::EditContactWindow *ui;
    Contact *mContact;

    void init();
};

#endif // EDITCONTACTWINDOW_H