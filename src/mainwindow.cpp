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

    boutonStart = new QPushButton(tr("&Start"));
    menu = new QMenu;

    boutonContacts = new QPushButton(tr("&Contacts"));
    boutonSettings = new QPushButton(tr("&Settings"));

    boutonStart->setToolTip("Demarre une nouvelle instance de communication");
    boutonContacts->setToolTip("Permet de gerer sa liste de contactes : Ajout;Retraits;Editions");
    boutonSettings->setToolTip("Permet de gerer les Principaux parametres de l'application tel que : port(défaut);version de Protocol;Peripheriques;etc...");

    layout = new QVBoxLayout();

    layout->addWidget(boutonStart);
    layout->addWidget(boutonContacts);
    layout->addWidget(boutonSettings);

    cManager = new ContactsManager();
    settings = new SettingsManager();
    starter  = new StarterManager();
    start = new Starter();


    namelist = new QStringList;

    set = new QSettings;

    qDebug()<<"!";

    set->beginGroup("Contacts");
    *namelist = set->childGroups();
    set->endGroup();

    actiongroup = new QActionGroup(menu);

    for(int i=0;i<namelist->length();i++)
    {
        actiongroup->addAction(namelist->at(i));
        connect(actiongroup, SIGNAL(triggered(QAction*)),this, SLOT(resolvestarter(QAction*)));
    }

    menu->addActions(actiongroup->actions());



    boutonStart->setMenu(menu);




    //connect(boutonStart     ,SIGNAL(clicked()), starter ,SLOT(starterWindow()));
    connect(boutonContacts  ,SIGNAL(clicked()), cManager,SLOT(contactsWindow()));
    connect(boutonSettings  ,SIGNAL(clicked()), settings,SLOT(settingsWindow()));

    setLayout(layout);
    show();
}
void MainWindow::resolvestarter(QAction* action)
{
    QString name = action->iconText();
    qDebug()<<name;
    start->opennewConnection(name);
}

MainWindow::~MainWindow()
{
    delete boutonStart, boutonContacts, boutonSettings;
    delete layout;
    delete cManager;
    delete settings;
    delete starter;
}
