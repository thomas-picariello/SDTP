#include "confwizard.h"
#include "ui_confwizard.h"

ConfWizard::ConfWizard(QPair<QByteArray, QByteArray> fileKey, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfWizard)
{
    ui->setupUi(this);

    m_RsaKeyRing = new RsaKeyring(&fileKey);

    QFile file(("ReadMe.txt"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);


        QString WelcomeTxt = file.readAll();


    m_MsgBox = new QMessageBox(QMessageBox::NoIcon,
                               "Welcome stranger",
                               WelcomeTxt,
                               QMessageBox::Ok);
    m_WarningBox = new QMessageBox(QMessageBox::Warning,
                                   "Warning",
                                   "One of the parameters seems to be wrong.",
                                   QMessageBox::Close);

    m_MsgBox->show();


    connect(m_MsgBox,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(show()));
}


bool ConfWizard::verifyKey(){

   // QByteArray key = ui->keyText->data();

    //if(m_RsaKeyRing->validatePrivateKey(key));


    return true;
}
bool ConfWizard::verifyPass(){
    QString pass1 = ui->PassLine->text(),pass2 = ui->ConfirmLine->text();

    if(pass1.isEmpty() && pass2.isEmpty())return true;
    else if (pass1.compare(pass2)==0)return true;
    else return false;

}
bool ConfWizard::verifyPort(){

    qDebug()<<ui->portBox->value();
    if(ui->portBox->value()<=65000 && ui->portBox->value() >= 1)return true;
    else return false;
}
bool ConfWizard::saveAll(){

    return true;
}


void ConfWizard::on_okButton_clicked()
{
    if(verifyKey() && verifyPass() && verifyPort()){
        if(saveAll())this->accept();
    }else m_WarningBox->show();
}

void ConfWizard::on_GenerateButton_clicked()
{

}

ConfWizard::~ConfWizard()
{
    delete ui;
}
