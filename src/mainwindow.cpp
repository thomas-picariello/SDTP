///////////////////////////////////////////////////////////////
/// Class "MainWindow" :                                    ///
///                                                         ///
/// Description of the main Window.                         ///
///                                                         ///
/// Placement of the Three Buttons (Start;Contacts;Settings)///
/// Layout Design ; here Horizontal                         ///
/// Connection from buttons to correspondent Classes :      ///
///                                                         ///
///->Starter                                                ///
///->ContactManager                                         ///
///->Settings                                               ///
///                                                         ///
///////////////////////////////////////////////////////////////

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent){

    m_boutonStart = new QPushButton(tr("&Start"));
    m_contactListMenu = new QMenu;

    m_boutonContacts = new QPushButton(tr("&Contacts"));
    m_boutonSettings = new QPushButton(tr("&Settings"));

    m_boutonStart->setToolTip("Demarre une nouvelle instance de communication");
    m_boutonContacts->setToolTip("Permet de gerer sa liste de contactes : Ajout;Retraits;Editions");
    m_boutonSettings->setToolTip("Permet de gerer les Principaux parametres de l'application tel que : port(défaut);version de Protocol;Peripheriques;etc...");

    m_windowLayout = new QVBoxLayout();

    m_windowLayout->addWidget(m_boutonStart);
    m_windowLayout->addWidget(m_boutonContacts);
    m_windowLayout->addWidget(m_boutonSettings);

    m_contactManager = new ContactsManager();
    m_settingsManager = new SettingsManager();
    m_starter = new Starter();


    m_contactNameList = new QStringList;

    QSettings settings;

    settings.beginGroup("Contacts");
    *m_contactNameList = settings.childGroups();
    settings.endGroup();

    m_contactListMenuActions = new QActionGroup(m_contactListMenu);

    for(int i=0;i<m_contactNameList->length();i++)
    {
        m_contactListMenuActions->addAction(m_contactNameList->at(i));

    }

    m_contactListMenu->addActions(m_contactListMenuActions->actions());
    connect(m_contactListMenuActions, SIGNAL(triggered(QAction*)),
            this, SLOT(resolvestarter(QAction*)));

    m_boutonStart->setMenu(m_contactListMenu);

    connect(m_boutonContacts  ,SIGNAL(clicked()), m_contactManager,SLOT(contactsWindow()));
    connect(m_boutonSettings  ,SIGNAL(clicked()), m_settingsManager,SLOT(settingsWindow()));

    setLayout(m_windowLayout);
    show();
}
void MainWindow::resolvestarter(QAction* action)
{
    QString name = action->iconText();
    m_starter->openConnection(name);
}

MainWindow::~MainWindow()
{
    delete m_boutonStart, m_boutonContacts, m_boutonSettings;
    delete m_contactListMenuActions;
    delete m_contactListMenu;
    delete m_contactNameList;
    delete m_windowLayout;
    delete m_contactManager;
    delete m_settingsManager;
    delete m_starter;
}
