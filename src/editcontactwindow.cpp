#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(Contact *contact, QWidget *parent):
    QWidget(parent), ui(new Ui::EditContactWindow)
{
    mContact = contact;
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));

    ui->name->setText(mContact->getName());
    ui->ip->setText(mContact->getIpAddress().toString());
    ui->port->setText(QString::number(mContact->getPort()));
    ui->key->setText(QString(mContact->getKey()));

    mPortValidator.setRange(0, 65535);
    ui->port->setValidator(&mPortValidator);

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
        mContact->setName(name);
        mContact->setPort(port.toUInt());
        //TODO use validators to decide to define ip or hostname
        mContact->setIpAddress(QHostAddress(ip));
        mContact->setKey(key.toUtf8());
        mContact->save();
        emit contactChanged();

        close();
        deleteLater();
    }
}

void EditContactWindow::cancel(){
    close();
    deleteLater();
}

EditContactWindow::~EditContactWindow(){
    delete ui, mContact;
}
