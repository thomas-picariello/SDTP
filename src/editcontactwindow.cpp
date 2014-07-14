#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(ContactDB *contactDB, QWidget *parent):
    QWidget(parent),
    mContactDB(contactDB),
    mContact(NULL),
    ui(new Ui::EditContactWindow)
{
    ui->setupUi(this);
    mPortValidator.setRange(0, 65535);
    ui->port->setValidator(&mPortValidator);
    ui->key->setWordWrapMode(QTextOption::WrapAnywhere);

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));
    connect(ui->hostslist_add_bt, SIGNAL(clicked()),
            this, SLOT(addHost()));
    connect(ui->hostslist_rm_bt, SIGNAL(clicked()),
            this, SLOT(removeHost()));
}

void EditContactWindow::open(Contact *contact){
    mContact = contact;
    ui->name->setText(mContact->getName());
    ui->hostslist->clear();
    foreach(QString host, mContact->getHostsList()){
        QListWidgetItem *item = new QListWidgetItem(host, ui->hostslist);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }
    ui->port->setText(QString::number(mContact->getPort()));
    ui->key->setText(QString(mContact->getKey().toBase64()));
    show();
    activateWindow();
}

void EditContactWindow::addHost(){
    QListWidgetItem *item = new QListWidgetItem("", ui->hostslist);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->hostslist->editItem(item);
}

void EditContactWindow::cancel(){
    close();
}

void EditContactWindow::save(){
    QString name = ui->name->text();
    QStringList hostsList;
    foreach(QListWidgetItem *item, ui->hostslist->findItems("*", Qt::MatchWildcard))
        hostsList.append(item->text());
    uint port = ui->port->text().toUInt();
    QString key = ui->key->toPlainText();

    if(name.isEmpty() || hostsList.isEmpty() || port==0 || key.isEmpty()){
        QMessageBox::warning(this, tr("Incomplete"), tr("Please fill all the fields"));
    }else{
        mContact->setName(name);
        mContact->setPort(port);
        mContact->setHostsList(hostsList);
        mContact->setKey(QByteArray::fromBase64(key.toUtf8()));
        mContactDB->save(mContact);
        close();
    }
}

void EditContactWindow::removeHost(){
    delete ui->hostslist->currentItem();
}

EditContactWindow::~EditContactWindow(){
    delete ui;
}
