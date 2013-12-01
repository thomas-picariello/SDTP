#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    QWidget *window = new QWidget();
    ui->setupUi(this);


            QPushButton *boutonStart = new QPushButton(tr("&Start"));
            QPushButton *boutonContacts = new QPushButton(tr("&Contacts"));
            QPushButton *boutonSettings = new QPushButton(tr("&Settings"));


            boutonStart->setToolTip("Demarre une nouvelle instance de communication");
            boutonContacts->setToolTip("Permet de gerer sa liste de contactes : Ajout;Retraits;Editions");
            boutonSettings->setToolTip("Permet de gerer les Principaux parametres de l'application tel que : port(défaut);version de Protocol;Peripheriques;etc...");

            QHBoxLayout *layout = new QHBoxLayout;

            layout->addWidget(boutonStart);
            layout->addWidget(boutonContacts);
            layout->addWidget(boutonSettings);

            window->setLayout(layout);
            window->show();

            Start = new QAction(("Start"),window);
            Contacts = new QAction(("Start"),window);
            Settings = new QAction(("Start"),window);





}

MainWindow::~MainWindow()
{
    delete ui;
}
