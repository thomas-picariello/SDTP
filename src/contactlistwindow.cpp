#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ContactListWindow)
{
    mEditContactWindow = NULL;
    mSettingsWindow = NULL;
//    mStarter = new Starter();
    ui->setupUi(this);

    ui->list->addItems(Contact::getAllNames());

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
    mEditContactWindow = new EditContactWindow();
    connect(mEditContactWindow, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}
void ContactListWindow::editContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        mEditContactWindow = new EditContactWindow(selectedName);
        connect(mEditContactWindow, SIGNAL(contactChanged()),
                this, SLOT(refreshList()));
    }
}
void ContactListWindow::removeContact(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        Contact contact(selectedName);
        contact.remove();
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
    ui->list->clear();
    ui->list->addItems(Contact::getAllNames());
}

ContactListWindow::~ContactListWindow()
{
    delete ui, mEditContactWindow, mSettingsWindow;
}
