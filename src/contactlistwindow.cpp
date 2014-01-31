#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ContactListWindow){
    mContactList = Contact::getContactList();
    ui->setupUi(this);

    qint16 listenPort = QSettings().value("Settings/port").toInt();

    mListener = new QTcpServer();
    mListener->listen(QHostAddress::Any, listenPort);


    refreshList();

    connect(mListener, SIGNAL(newConnection()),
            this,SLOT(acceptConnection()));
    connect(ui->add, SIGNAL(clicked()),
            this, SLOT(addContact()));
    connect(ui->edit, SIGNAL(clicked()),
            this, SLOT(editContact()));
    connect(ui->remove, SIGNAL(clicked()),
            this, SLOT(removeContact()));
    connect(ui->connect, SIGNAL(clicked()),
            this, SLOT(connectToContact()));
    connect(ui->list, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(connectToContact()));
    connect(ui->settings, SIGNAL(clicked()),
            this, SLOT(openSettingsWindow()));
    connect(ui->exit, SIGNAL(clicked()),
            this, SLOT(exitApp()));

    show();
}

void ContactListWindow::acceptConnection(){
    m_ManagerList.append(new NetworkManager(mListener->nextPendingConnection()));
}

void ContactListWindow::addContact(){
    EditContactWindow *ecw = new EditContactWindow(Contact());
    connect(ecw, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}

void ContactListWindow::editContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        EditContactWindow *ecw = new EditContactWindow(Contact::findByName(selectedName));
        connect(ecw, SIGNAL(contactChanged()),
                this, SLOT(refreshList()));
    }
}

void ContactListWindow::removeContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        Contact contact = Contact::findByName(selectedName);
        contact.erase();
        refreshList();
    }
}

void ContactListWindow::connectToContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        m_ManagerList.append(new NetworkManager(Contact::findByName(selectedName)));
    }
}

void ContactListWindow::openSettingsWindow(){
    SettingsWindow *setWin = new SettingsWindow();
    connect(setWin, SIGNAL(settingsUpdated()),
            this, SLOT(restartListener()));
}

void ContactListWindow::exitApp(){
    exit(0);
}

void ContactListWindow::refreshList(){
    mContactList = Contact::getContactList();
    QStringList nameList;
    foreach(Contact contact, mContactList)
        nameList.append(contact.getName());
    ui->list->clear();
    ui->list->addItems(nameList);
}

void ContactListWindow::restartListener(){
    qint16 listenPort = QSettings().value("Settings/port").toInt();
    qDebug()<<"Listener restart with port:"<<listenPort;
    mListener->close();
    mListener->listen(QHostAddress::Any, listenPort);
}

ContactListWindow::~ContactListWindow(){
    delete ui;
}
