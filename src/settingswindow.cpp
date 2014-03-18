#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QByteArray *fileKey, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mFileKey(fileKey),
    mKeyring(mFileKey),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat)),
    mProgress(new QProgressDialog("Generating Keypair", "Cancel", 0, 0, this, Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint))
{
    ui->setupUi(this);

    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_pubkey_input->setText(mSettings->value("keyring/public_key").toString());

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    mProgress->setModal(true);

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

}

void SettingsWindow::rsaGenerate(){
    mKeyring.generateKeypair();
    mProgress->setVisible(true);
}

void SettingsWindow::rsaKeyGenFinished(){
    ui->rsa_privkey_input->setText(mKeyring.getPrivateKey()->toBase64());
    ui->rsa_pubkey_input->setText(mKeyring.getPublicKey()->toBase64());
    mProgress->setVisible(false);
}

void SettingsWindow::rsaImport(){

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
    delete ui, mSettings, mProgress;
}
