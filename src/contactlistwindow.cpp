#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ContactListWindow){
    ui->setupUi(this);

    qint16 listenPort = QSettings().value("Settings/port").toInt();

    mListener = new QTcpServer(this);
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
    connect(ui->list, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(connectToContact()));
    connect(ui->settings, SIGNAL(clicked()),
            this, SLOT(openSettingsWindow()));
    connect(ui->exit, SIGNAL(clicked()),
            this, SLOT(exitApp()));

    show();
}

void ContactListWindow::acceptConnection(){
    mManagerList.append(new NetworkManager(mListener->nextPendingConnection(), this));
}

void ContactListWindow::addContact(){
    EditContactWindow *ecw = new EditContactWindow(new Contact(this));
    connect(ecw, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}

void ContactListWindow::editContact(){
    if(getSelectedContact()){
        EditContactWindow *ecw = new EditContactWindow(getSelectedContact());
        connect(ecw, SIGNAL(contactChanged()),
                this, SLOT(refreshList()));
    }
}

void ContactListWindow::removeContact(){
    if(getSelectedContact()){
        getSelectedContact()->erase();
        refreshList();
    }
}

void ContactListWindow::connectToContact(){
    if(getSelectedContact()){
        mManagerList.append(new NetworkManager(getSelectedContact()));
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
    mContactList = ContactFactory::getContactList();
    qDeleteAll(mItemList);
    mItemList.clear();
    foreach(Contact *contact, mContactList){
        QListWidgetItem *item = new QListWidgetItem(contact->getName());
        item->setData(IdRole, contact->getId());
        mItemList.append(item);
    }
    ui->list->clear();
    for(int i=0; i<mItemList.size();i++){
        ui->list->addItem(mItemList.at(i));
    }
}

void ContactListWindow::restartListener(){
    qint16 listenPort = QSettings().value("Settings/port").toInt();
    mListener->close();
    mListener->listen(QHostAddress::Any, listenPort);
}

Contact* ContactListWindow::getSelectedContact(){
    QListWidgetItem *currentItem = ui->list->currentItem();
    if(currentItem){
        int currentId = currentItem->data(IdRole).toInt();
        return ContactFactory::findById(currentId, this);
    }
    return NULL;
}

ContactListWindow::~ContactListWindow(){
    qDeleteAll(mItemList);
    qDeleteAll(mContactList);
    qDeleteAll(mManagerList);
    delete ui, mListener;
}
