#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QString>
#include <QMenu>
#include <QSettings>
#include <QAction>
#include <QStringList>
#include "contactsmanager.h"
#include "settingsmanager.h"
#include "startermanager.h"
#include "soundsampler.h"
#include "starter.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void resolvestarter(QAction*);

private:
    QPushButton *boutonStart, *boutonContacts, *boutonSettings;
    QMenu *menu;
    QSettings *set;
    QAction *action;
    QActionGroup *actiongroup;
    QStringList *namelist;
    QVBoxLayout *layout;
    QWidget *window;
    ContactsManager *cManager;
    SettingsManager *settings;
    StarterManager *starter;
    Starter *start;
};

#endif // MAINWINDOW_H

