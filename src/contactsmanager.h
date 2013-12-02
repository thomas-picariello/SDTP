#ifndef CONTACTSMANAGER_H
#define CONTACTSMANAGER_H


#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>


class ContactsManager: public QWidget
{

    Q_OBJECT

public:
    ContactsManager();
    ~ContactsManager();

public slots:
     void contactsWindow();

private:
    QWidget *Contactwindow;
    QPushButton *About;
};

#endif // CONTACTSMANAGER_H
