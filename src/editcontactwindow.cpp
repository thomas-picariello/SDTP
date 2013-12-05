#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(QString name, QWidget *parent) : QWidget(parent),ui(new Ui::EditContactWindow)
{
    ui->setupUi(this);

    if(name.isEmpty()){
        mContact = new Contact();
    }else{
        mContact = new Contact(name);
        ui->name->setText(name);
        ui->ip->setText(mContact->getIp());
        ui->port->setText(mContact->getPort());
        ui->key->setText(QString(mContact->getKey()));
    }

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));
    this->show();
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
            qDebug() << mContact->getName();
            emit contactChanged();
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
