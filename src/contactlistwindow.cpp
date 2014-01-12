#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ContactListWindow)
{
    mEditContactWindow = NULL;
    mSettingsWindow = NULL;
    //mStarter = new Starter();
    mContactList = Contact::getContactList();
    ui->setupUi(this);

    refreshList();

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

void ContactListWindow::addContact(){
    mEditContactWindow = new EditContactWindow(Contact());
    connect(mEditContactWindow, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}
void ContactListWindow::editContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        mEditContactWindow = new EditContactWindow(Contact::findByName(selectedName));
        connect(mEditContactWindow, SIGNAL(contactChanged()),
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
        m_ManagerList.append(new NetworkManager(selectedName));
       // mStarter->openConnection(selectedName);
    }
}
void ContactListWindow::openSettingsWindow(){
    mSettingsWindow = new SettingsWindow();
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

ContactListWindow::~ContactListWindow()
{
    delete ui, mEditContactWindow, mSettingsWindow;
}
