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
    QPushButton *boutonStart, *boutonContacts, *boutonSettings;
    QVBoxLayout *layout;
    QWidget *window;
    ContactsManager *cManager;
    SettingsManager *settings;
    StarterManager *starter;
};

#endif // MAINWINDOW_H

