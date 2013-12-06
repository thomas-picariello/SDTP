#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(QWidget *parent): QWidget(parent), ui(new Ui::EditContactWindow)
{
    mContact = new Contact();
    this->init();

    this->show();
}
EditContactWindow::EditContactWindow(QString name, QWidget *parent): QWidget(parent), ui(new Ui::EditContactWindow)
{
    mContact = new Contact(name);
    this->init();

    ui->name->setText(mContact->getName());
    ui->ip->setText(mContact->getIp());
    ui->port->setText(mContact->getPort());
    ui->key->setText(QString(mContact->getKey()));

    this->show();
}

void EditContactWindow::init(){
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    this->connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));
}

void EditContactWindow::save(){
    QString name = ui->name->text();
    QString ip = ui->ip->text();
    QString port = ui->port->text();
    QString key = ui->key->toPlainText();

    if(name.isEmpty() || ip.isEmpty() || port.isEmpty() || key.isEmpty()){
        QMessageBox::warning(this, "Incomplete", "Please fill all the fields");
    }else{
        //graph             //obj                   //reg
        if(!name.contains(mContact->getName()) && Contact::nameExists(name)){
            QMessageBox::critical(this, "Error", "This name already exists");
        }else{
            mContact->setName(name);
            mContact->setPort(port);
            mContact->setIp(ip);
            mContact->setKey(key.toUtf8());
            emit this->contactChanged();

            this->close();
            this->deleteLater();
        }
    }
}
void EditContactWindow::cancel(){
    this->close();
    this->deleteLater();
}

EditContactWindow::~EditContactWindow()
{
    delete ui, mContact;
}
