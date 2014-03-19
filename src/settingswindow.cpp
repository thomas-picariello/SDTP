#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QByteArray *fileKey, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mFileKey(fileKey),
    mKeyring(mFileKey),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat))
{
    ui->setupUi(this);

    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_pubkey_input->setText(mSettings->value("keyring/public_key").toString());
    ui->rsa_generate_pb->setMaximumWidth(ui->rsa_generate_bt->width());
    ui->rsa_generate_pb->setVisible(false);

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    connect(ui->save_bt, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel_bt, SIGNAL(clicked()),
            this, SLOT(cancel()));

    connect(ui->rsa_generate_bt, SIGNAL(clicked()),
            this, SLOT(rsaGenerate()));
    connect(ui->rsa_export_bt, SIGNAL(clicked()),
            this, SLOT(rsaExport()));
    connect(ui->rsa_import_bt, SIGNAL(clicked()),
            this, SLOT(rsaImport()));
    connect(&mKeyring, SIGNAL(keyGenerationFinished()),
            this, SLOT(rsaKeyGenFinished()));
    show();
}

void SettingsWindow::cancel(){
    close();
    deleteLater();
}

void SettingsWindow::rsaExport(){
    QString filename = QFileDialog::getSaveFileName(this, "Save Key", "",
                                                          "Key files (*.key *.pem);;Other files (*.*)");
}

void SettingsWindow::rsaGenerate(){
    ui->rsa_generate_pb->setVisible(true);
    ui->rsa_generate_bt->setVisible(false);
    mKeyring.generateKeypair();
}

void SettingsWindow::rsaKeyGenFinished(){
    ui->rsa_generate_pb->setVisible(false);
    ui->rsa_generate_bt->setVisible(true);
    ui->rsa_privkey_input->setText(mKeyring.getPrivateKey()->toBase64());
    ui->rsa_pubkey_input->setText(mKeyring.getPublicKey()->toBase64());
}

void SettingsWindow::rsaImport(){
    QString filename = QFileDialog::getOpenFileName(this, "Select Keys", "",
                                                          "Key files (*.key *.pem);;Other files (*.*)");
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

SettingsWindow::~SettingsWindow(){
    delete ui, mSettings;
}
