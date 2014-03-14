#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat))
{
    ui->setupUi(this);

    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_pubkey_input->setText(mSettings->value("keyring/public_key").toString());

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    connect(ui->save_bt, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel_bt, SIGNAL(clicked()),
            this, SLOT(cancel()));
    show();
}

void SettingsWindow::save(){
    if(ui->net_port_input->text().isEmpty() ||
       ui->rsa_pubkey_input->toPlainText().isEmpty()){
        QMessageBox::warning(this, "Incomplete", "Please fill all the fields");
    }else{
        mSettings->setValue("network/listen_port", ui->net_port_input->text());
        mSettings->setValue("keyring/public_key", ui->rsa_pubkey_input->toPlainText().toUtf8());
        emit settingsUpdated();
        close();
        deleteLater();
    }
}

void SettingsWindow::cancel(){
    close();
    deleteLater();
}

SettingsWindow::~SettingsWindow(){
    delete ui, mSettings;
}
