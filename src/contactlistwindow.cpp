#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(ContactDB *contactDB, QPair<QByteArray, QByteArray> *fileKey, QWidget *parent):
    QWidget(parent),
    mContactDB(contactDB),
    mFileKey(fileKey),
    ui(new Ui::ContactListWindow)
{
    ui->setupUi(this);

    qint16 listenPort = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();

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
}

void ContactListWindow::acceptConnection(){
    mManagerList.append(new NetworkManager(mListener->nextPendingConnection(),mContactDB, this));
}

void ContactListWindow::addContact(){
    EditContactWindow *ecw = new EditContactWindow(new Contact(), mContactDB);
    connect(ecw, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}

void ContactListWindow::editContact(){
    if(getSelectedContact()){
        EditContactWindow *ecw = new EditContactWindow(getSelectedContact(), mContactDB);
        connect(ecw, SIGNAL(contactChanged()),
                this, SLOT(refreshList()));
    }
}

void ContactListWindow::removeContact(){
    QListWidgetItem *currentItem = ui->list->currentItem();
    if(currentItem){
        int currentId = currentItem->data(IdRole).toInt();
        mContactDB->erase(currentId);
        refreshList();
    }
}

void ContactListWindow::connectToContact(){
    if(getSelectedContact()){
        mManagerList.append(new NetworkManager(getSelectedContact(), mContactDB, this));
    }
}

void ContactListWindow::openSettingsWindow(){
    SettingsWindow *setWin = new SettingsWindow(mFileKey);
    connect(setWin, SIGNAL(settingsUpdated()),
            this, SLOT(restartListener()));
}

void ContactListWindow::exitApp(){
    exit(0);
}

void ContactListWindow::refreshList(){
    mContactList = mContactDB->getAllContacts();
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
    qint16 listenPort = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();
    mListener->close();
    mListener->listen(QHostAddress::Any, listenPort);
}

Contact* ContactListWindow::getSelectedContact(){
    QListWidgetItem *currentItem = ui->list->currentItem();
    if(currentItem){
        int currentId = currentItem->data(IdRole).toInt();
        return mContactDB->findById(currentId);
    }
    return NULL;
}

ContactListWindow::~ContactListWindow(){
    qDeleteAll(mItemList);
    qDeleteAll(mContactList);
    qDeleteAll(mManagerList);
    delete ui, mListener;
}
