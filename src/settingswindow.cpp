#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat))
{
    ui->setupUi(this);

    ui->port->setText(mSettings->value("network/listen_port").toString());
    ui->key->setText(mSettings->value("keyring/public_key").toString());

    mPortValidator.setRange(0, 65535);
    ui->port->setValidator(&mPortValidator);

    connect(ui->save, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(cancel()));
    show();
}

void SettingsWindow::save(){
    if(ui->port->text().isEmpty() ||
       ui->key->toPlainText().isEmpty()){
        QMessageBox::warning(this, "Incomplete", "Please fill all the fields");
    }else{
        mSettings->setValue("network/listen_port", ui->port->text());
        mSettings->setValue("keyring/public_key", ui->key->toPlainText().toUtf8());
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
