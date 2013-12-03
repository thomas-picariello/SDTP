#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QString>
#include "contactsmanager.h"
#include "settingsmanager.h"
#include "startermanager.h"
#include "soundsampler.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

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

