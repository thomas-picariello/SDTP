#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ContactListWindow)
{
    mEditContactWindow = 0;
    mStarter = new Starter();
    ui->setupUi(this);

    ui->list->addItems(Contact::getAllNames());

    connect(ui->add, SIGNAL(clicked()),
            this, SLOT(onAddBtClick()));
    connect(ui->edit, SIGNAL(clicked()),
            this, SLOT(onEditBtClick()));
    connect(ui->remove, SIGNAL(clicked()),
            this, SLOT(onRemoveBtClick()));
    connect(ui->connect, SIGNAL(clicked()),
            this, SLOT(onConnectBtClick()));
    connect(ui->exit, SIGNAL(clicked()),
            this, SLOT(onExitBtClick()));

    show();
}

void ContactListWindow::onAddBtClick(){
    mEditContactWindow = new EditContactWindow();
    connect(mEditContactWindow, SIGNAL(contactChanged()),
            this, SLOT(refreshList()));
}
void ContactListWindow::onEditBtClick(){

}
void ContactListWindow::onRemoveBtClick(){

}
void ContactListWindow::onConnectBtClick(){
    QList<QListWidgetItem*> selectedLines = ui->list->selectedItems();
    if(selectedLines.count()>0){
        QString selectedName = selectedLines.first()->text();
        mStarter->openConnection(selectedName);
    }
}
void ContactListWindow::onExitBtClick(){
    exit(0);
}
void ContactListWindow::refreshList(){
    ui->list->clear();
    ui->list->addItems(Contact::getAllNames());
}

ContactListWindow::~ContactListWindow()
{
    delete ui, mEditContactWindow, mStarter;
}
