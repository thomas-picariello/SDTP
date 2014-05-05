#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(Contact *contact, ContactDB *contactDB, QWidget *parent):
    QWidget(parent), ui(new Ui::EditContactWindow)
{
    mContact = contact;
    mContactDB = contactDB;
    mContact->setParent(this);
    ui->setupUi(this);

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));

    ui->name->setText(mContact->getName());
    foreach(QString host, mContact->getHostsList()){
        QListWidgetItem *item = new QListWidgetItem(host, ui->hostslist);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }
    ui->port->setText(QString::number(mContact->getPort()));
    ui->key->setText(QString(mContact->getKey().toBase64()));

    mPortValidator.setRange(0, 65535);
    ui->port->setValidator(&mPortValidator);

    connect(ui->hostslist_add_bt, SIGNAL(clicked()),
            this, SLOT(addHost()));
    connect(ui->hostslist_rm_bt, SIGNAL(clicked()),
            this, SLOT(removeHost()));

    this->show();
}

void EditContactWindow::addHost(){
    QListWidgetItem *item = new QListWidgetItem("", ui->hostslist);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->hostslist->editItem(item);
}

void EditContactWindow::cancel(){
    close();
    deleteLater();
}

void EditContactWindow::save(){
    QString name = ui->name->text();
    QStringList hostsList;
    foreach(QListWidgetItem *item, ui->hostslist->findItems("*", Qt::MatchWildcard))
        hostsList.append(item->text());
    QString port = ui->port->text();
    QString key = ui->key->toPlainText();

    if(name.isEmpty() || hostsList.isEmpty() || port == 0 || key.isEmpty()){
        QMessageBox::warning(this, tr("Incomplete"), tr("Please fill all the fields"));
    }else{
        mContact->setName(name);
        mContact->setPort(port.toUInt());
        mContact->setHostsList(hostsList);
        mContact->setKey(QByteArray::fromBase64(key.toUtf8()));
        int id = mContactDB->write(mContact);
        if(mContact->getId() == 0)
            emit contactEvent(id, Contact::Added);
        else
            emit contactEvent(id, Contact::Updated);
        close();
        deleteLater();
    }
}

void EditContactWindow::removeHost(){
    delete ui->hostslist->currentItem();
}

EditContactWindow::~EditContactWindow(){
    delete ui, mContact;
}
