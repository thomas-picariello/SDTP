#include "contactlistwindow.h"
#include "ui_contactlistwindow.h"

ContactListWindow::ContactListWindow(ContactDB *contactDB, RsaKeyring *keyring, QPair<QByteArray, QByteArray> *fileKey, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ContactListWindow),
    mContactDB(contactDB),
    mSettingsWindow(fileKey, keyring, mContactDB),
    mEditContactWindow(mContactDB)
{
    ui->setupUi(this);
    //scale window
    setMinimumHeight(minimumHeight()*(logicalDpiX()/96));
    setMinimumWidth (minimumWidth() *(logicalDpiX()/96));

    //scale buttons
    ui->add->setMinimumHeight(ui->add->minimumHeight()*(logicalDpiX()/96));
    ui->add->setMinimumWidth(ui->add->minimumWidth() * (logicalDpiX()/96));
    ui->settings->setMinimumHeight(ui->settings->minimumHeight()*(logicalDpiX()/96));
    ui->settings->setMinimumWidth(ui->settings->minimumWidth() * (logicalDpiX()/96));

    connect(ui->add, &QPushButton::clicked,
            this, &ContactListWindow::addContact);
    connect(ui->list, &QListWidget::itemClicked,
            this, &ContactListWindow::listItemClicked);
    connect(ui->settings, &QPushButton::clicked,
            this, &ContactListWindow::openSettingsWindow);
    connect(&mSettingsWindow, &SettingsWindow::settingsUpdated,
            this, &ContactListWindow::settingsUpdated);
    connect(mContactDB, &ContactDB::contactEvent,
            this, &ContactListWindow::refreshList);

    refreshList();
    show();
}

void ContactListWindow::addContact(){
    mEditContactWindow.open(new Contact());
}

void ContactListWindow::listItemClicked(QListWidgetItem *currentItem){
    QList<QListWidgetItem*> itemList = ui->list->findItems("*", Qt::MatchWildcard);
    foreach(QListWidgetItem *item, itemList){
           dynamic_cast<ContactActionsWidget*>(ui->list->itemWidget(item))->hide();
    }
    dynamic_cast<ContactActionsWidget*>(ui->list->itemWidget(currentItem))->show();
}

void ContactListWindow::openSettingsWindow(){
    mSettingsWindow.show();
    mSettingsWindow.activateWindow();
}

void ContactListWindow::onContactAction(int contactId, ContactDB::Action action){
    switch(action){
    case ContactDB::Edit:
        mEditContactWindow.open(mContactDB->findById(contactId));
        break;
    case ContactDB::Delete:
        if(QMessageBox::warning(this,
                                tr("Confirmation"),
                                tr("Are you sure want to delete this contact ?"),
                                QMessageBox::No,
                                QMessageBox::Yes) == QMessageBox::Yes)
            mContactDB->erase(contactId);
        break;
    }
    refreshList();
}

void ContactListWindow::onStartApp(int contactId, AppType type){
    emit startApp(mContactDB->findById(contactId), type);
}

//TODO: split in addContact(Contact* contact), updateContactName(Contact* contact), deleteContact(int id)
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
            setContactStatusIcon(item, contact->getStatus());
            connect(contact, &Contact::statusChanged,
                    this, &ContactListWindow::updateContactStatus);
            ui->list->addItem(item);
            ContactActionsWidget *itemWidget = new ContactActionsWidget(contact->getId());
            //add apps items
            AppLauncherItem* messengerLauncher = new AppLauncherItem();
            messengerLauncher->setAppType(Messenger);
            messengerLauncher->setTitle(tr("Messenger"));
            messengerLauncher->setIconUrls(":/icons/messenger_normal",":/icons/messenger_hover",":/icons/messenger_pressed");
            itemWidget->addAppLauncherItem(messengerLauncher);
            AppLauncherItem* callLauncher = new AppLauncherItem();
            callLauncher->setAppType(VoIP);
            callLauncher->setTitle(tr("Messenger"));
            callLauncher->setIconUrls(":/icons/call_normal",":/icons/call_hover",":/icons/call_pressed");
            itemWidget->addAppLauncherItem(callLauncher);
            AppLauncherItem* videoLauncher = new AppLauncherItem();
            videoLauncher->setAppType(VideoStreamer);
            videoLauncher->setTitle(tr("Messenger"));
            videoLauncher->setIconUrls(":/icons/video_normal",":/icons/video_hover",":/icons/video_pressed");
            itemWidget->addAppLauncherItem(videoLauncher);

            ui->list->setItemWidget(item, itemWidget);
            ui->list->itemWidget(item)->hide();
            connect(itemWidget, &ContactActionsWidget::contactAction,
                    this, &ContactListWindow::onContactAction);
            connect(itemWidget, &ContactActionsWidget::startApp,
                    this, &ContactListWindow::onStartApp);
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

void ContactListWindow::updateContactStatus(){
    Contact* contact = dynamic_cast<Contact*>(sender());
    if(contact){
        QListWidgetItem *item = findItemByContactId(contact->getId());
        if(item){
            setContactStatusIcon(item, contact->getStatus());
            dynamic_cast<ContactActionsWidget*>(ui->list->itemWidget(item))->setAppLauncherHidden(contact->getStatus() == Contact::Offline);
        }
    }
}

Contact* ContactListWindow::getSelectedContact(){
    QListWidgetItem *currentItem = ui->list->currentItem();
    if(currentItem){
        int currentId = currentItem->data(IdRole).toInt();
        return mContactDB->findById(currentId);
    }
    return NULL;
}

void ContactListWindow::setContactStatusIcon(QListWidgetItem *item, Contact::Status status){
    float iconScaleFactor = logicalDpiX()/96;
    QPixmap statusIcon(32 * iconScaleFactor,
                       32 * iconScaleFactor);
    statusIcon.fill(Qt::transparent);
    QPainter painter(&statusIcon);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::transparent));

    if(status == Contact::Offline)
        painter.setBrush(QBrush(Qt::lightGray));
    else if(status == Contact::Online)
        painter.setBrush(QBrush(Qt::green));
    else if(status == Contact::Busy)
        painter.setBrush(QBrush(Qt::red));
    else if(status == Contact::Away)
        painter.setBrush(QBrush(QColor(255, 192, 0)));

    painter.drawEllipse(QPoint(16 * iconScaleFactor, 16 * iconScaleFactor),
                        (int)(5*iconScaleFactor), (int)(5*iconScaleFactor));

    item->setData(Qt::DecorationRole, statusIcon);
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
    delete ui;
}
