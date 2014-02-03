#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :QWidget(parent), ui(new Ui::SettingsWindow){
    ui->setupUi(this);

    mSettings.beginGroup("Settings");

    ui->port->setText(mSettings.value("port").toString());
    ui->key->setText(mSettings.value("key").toString());

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
        mSettings.setValue("port", ui->port->text());
        mSettings.setValue("key", ui->key->toPlainText().toUtf8());
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
    mSettings.endGroup();
    delete ui;
}
