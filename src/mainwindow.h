#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>
#include "contactsmanager.h"
#include "settingsManager.h"
#include "starterManager.h"

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
   ContactsManager *cManager;
   Settings *settings;
   Starter *starter;
};

#endif // MAINWINDOW_H

