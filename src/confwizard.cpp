#include "confwizard.h"
#include "ui_confwizard.h"

ConfWizard::ConfWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfWizard)
{
    ui->setupUi(this);






}

ConfWizard::~ConfWizard()
{
    delete ui;
}

void ConfWizard::on_KeyButton_clicked()
{

}

void ConfWizard::on_NetworkButton_clicked()
{

}

void ConfWizard::on_ContactsButton_clicked()
{

}

void ConfWizard::on_PasswordButton_clicked()
{

}
