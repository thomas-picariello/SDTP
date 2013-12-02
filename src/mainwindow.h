#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>
#include "contactsmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    QPushButton *boutonStart, *boutonContacts, *boutonSettings;
    QHBoxLayout *layout;
    QWidget *window;
    QAction *Start, *Contacts, *Settings;
   ContactsManager *cManager;
};

#endif // MAINWINDOW_H

