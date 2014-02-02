#include "editcontactwindow.h"
#include "ui_editcontactwindow.h"

EditContactWindow::EditContactWindow(Contact *contact, QWidget *parent):
    QWidget(parent), ui(new Ui::EditContactWindow)
{
    mContact = contact;
    ui->setupUi(this);

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));

    ui->name->setText(mContact->getName());
    if(mContact->getHostName().size() > 0)
        ui->host->setText(mContact->getHostName());
    else
        ui->host->setText(mContact->getIpAddress().toString());
    ui->port->setText(QString::number(mContact->getPort()));
    ui->key->setText(QString(mContact->getKey()));

    mPortValidator.setRange(0, 65535);
    ui->port->setValidator(&mPortValidator);

    this->show();
}

void EditContactWindow::save(){
    QString name = ui->name->text();
    QString host = ui->host->text();
    QString port = ui->port->text();
    QString key = ui->key->toPlainText();



    if(name.isEmpty() || host.isEmpty() || port.isEmpty() || key.isEmpty()){
        QMessageBox::warning(this, "Incomplete", "Please fill all the fields");
    }else{
        mContact->setName(name);
        mContact->setPort(port.toUInt());
        if(isIpAddress(host))
            mContact->setIpAddress(QHostAddress(host));
        else
            mContact->setHostName(host);
        mContact->setKey(key.toUtf8());
        close();
        emit contactChanged();
        if(mContact->isResolving()){
            connect(mContact, SIGNAL(resolveResult(QString)),
                    this, SLOT(saveAfterResolving()));
            qDebug()<<"isResolving";
        }else{
            deleteLater();
        }
    }
}

void EditContactWindow::cancel(){
    close();
    deleteLater();
}

bool EditContactWindow::isIpAddress(QString host){
    int pos = 0;
    QRegExp ipv4("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|$)){4}"); //TODO: remove trailing 0
    QRegExp ipv6("/^(((?=.*(::))(?!.*\\3.+\\3))\\3?|([\\dA-F]{1,4}(\\3|:\\b|$)|\\2))(?4){5}((?4){2}|(((2[0-4]|1\\d|[1-9])?\\d|25[0-5])\\.?\\b){4})\\z/i");
    mIpValidator.setRegExp(QRegExp(ipv4));
    if(mIpValidator.validate(host, pos) == QValidator::Acceptable){
        qDebug()<<"ipv4";
        return true;
    }
    mIpValidator.setRegExp(ipv6);
    if(mIpValidator.validate(host, pos) == QValidator::Acceptable){
        qDebug()<<"ipv6";
        return true;
    }
    qDebug()<<"hostname";
    return false;
}

void EditContactWindow::saveAfterResolving(){
    qDebug()<<"Resolving finished";
    mContact->save();
    deleteLater();
}

EditContactWindow::~EditContactWindow(){
    delete ui, mContact;
}
