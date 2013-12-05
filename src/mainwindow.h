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
    QPushButton  *m_boutonStart,*m_boutonContacts, *m_boutonSettings;
    QMenu *m_contactListMenu;
    QActionGroup *m_contactListMenuActions;
    QStringList *m_contactNameList;
    QVBoxLayout *m_windowLayout;
    ContactsManager *m_contactManager;
    SettingsManager *m_settingsManager;
    Starter *m_starter;
};

#endif // MAINWINDOW_H

