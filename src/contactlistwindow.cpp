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
    connect(ui->list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(listSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->list, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(listItemClicked(QListWidgetItem*)));
    connect(ui->settings, SIGNAL(clicked()),
            this, SLOT(openSettingsWindow()));

    show();
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

void ContactListWindow::listSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous){
    if(previous)
        dynamic_cast<ContactItemWidget*>(ui->list->itemWidget(previous))->hide();
}

void ContactListWindow::listItemClicked(QListWidgetItem *item){
    dynamic_cast<ContactItemWidget*>(ui->list->itemWidget(item))->show();
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

void ContactListWindow::onListItemAction(int id, ContactItemWidget::Action action){
    switch(action){
    case ContactItemWidget::CallAction:
        break;
    case ContactItemWidget::MessengerAction:
        connectToContact();
        break;
    case ContactItemWidget::EditAction:
        editContact();
        break;
    case ContactItemWidget::DeleteAction:
        deleteContact();
        break;
    }
}

void ContactListWindow::refreshList(){
    QList<Contact*> contactList = mContactDB->getAllContacts();

    //check if contact added or modified
    foreach(Contact *contact, contactList){
        QListWidgetItem *contactListItem = findItemByContactId(contact->getId());
        if(contactListItem){
            //contact already in the item list
            contactListItem->setText(contact->getName());
        }else{
            //new contact
            QListWidgetItem *item = new QListWidgetItem(contact->getName());
            item->setData(IdRole, contact->getId());
            setContactStatusIcon(item, Offline);
            ui->list->addItem(item);
            ContactItemWidget *itemWidget = new ContactItemWidget(contact->getId());
            ui->list->setItemWidget(item, itemWidget);
            itemWidget->hide();
            connect(itemWidget, SIGNAL(actionTriggered(int,ContactItemWidget::Action)),
                    this, SLOT(onListItemAction(int,ContactItemWidget::Action)));
        }
    }

    //check if contact deleted
    QList<QListWidgetItem*> itemList = ui->list->findItems("*", Qt::MatchWildcard);
    foreach(QListWidgetItem *item, itemList){
        int id = item->data(IdRole).toInt();
        bool found = false;
        foreach(Contact *contact, contactList){
            if(contact->getId() == id)
                found = true;
        }
        if(!found){
            delete item;
        }
    }

    //clear selection and focus
    ui->list->clearSelection();
    ui->list->clearFocus();
}

void ContactListWindow::deleteContact(){
    QListWidgetItem *currentItem = ui->list->currentItem();
    if(currentItem){
        int currentId = currentItem->data(IdRole).toInt();
        mContactDB->erase(currentId);
        refreshList();
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

void ContactListWindow::setContactStatusIcon(QListWidgetItem *item, Status status){
    QPixmap statusIcon(32,32);
    statusIcon.fill(Qt::transparent);
    QPainter painter(&statusIcon);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::transparent));

    if(status == Offline)
        painter.setBrush(QBrush(Qt::lightGray));
    else if(status == Online)
        painter.setBrush(QBrush(Qt::green));
    else if(status == Busy)
        painter.setBrush(QBrush(Qt::red));
    else if(status == Away)
        painter.setBrush(QBrush(QColor(255, 192, 0)));

    painter.drawEllipse(QPoint(16,16), 5, 5);

    item->setData(Qt::DecorationRole, statusIcon);
}

void ContactListWindow::setContactStatusIcon(int id, Status status){
    QListWidgetItem *item = findItemByContactId(id);
    if(item)
        setContactStatusIcon(item, status);
}

void ContactListWindow::setContactStatusIcon(Contact *contact, Status status){
    setContactStatusIcon(contact->getId(), status);
}

QListWidgetItem* ContactListWindow::findItemByContactId(int id){
    QList<QListWidgetItem*> itemList = ui->list->findItems("*", Qt::MatchWildcard);
    foreach(QListWidgetItem *item, itemList){
        if(item->data(IdRole).toInt() == id)
            return item;
    }
    return NULL;
}

ContactListWindow::~ContactListWindow(){
    qDeleteAll(mManagerList);
    delete ui, mListener;
}
