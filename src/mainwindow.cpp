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
#include "ui_mainwindow.h"
#include "contactsmanager.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    window = new QWidget();
    ui->setupUi(this);


     boutonStart = new QPushButton(tr("&Start"));
     boutonContacts = new QPushButton(tr("&Contacts"));
     boutonSettings = new QPushButton(tr("&Settings"));


    boutonStart->setToolTip("Demarre une nouvelle instance de communication");
    boutonContacts->setToolTip("Permet de gerer sa liste de contactes : Ajout;Retraits;Editions");
    boutonSettings->setToolTip("Permet de gerer les Principaux parametres de l'application tel que : port(défaut);version de Protocol;Peripheriques;etc...");

     layout = new QHBoxLayout;

    layout->addWidget(boutonStart);
    layout->addWidget(boutonContacts);
    layout->addWidget(boutonSettings);

    window->setLayout(layout);
    window->show();


    cManager = new ContactsManager();

//            connect(boutonStart,SIGNAL(clicked()),Starter, SLOT(starterWindow()));
            QObject::connect(boutonContacts,SIGNAL(clicked()),cManager,SLOT(contactsWindow()));
    //        connect(boutonSettings,SIGNAL(clicked()),Settings,SLOT(settingsWindow()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
