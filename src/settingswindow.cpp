#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QPair<QByteArray, QByteArray> *fileKey, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mFileKey(fileKey),
    mKeyring(mFileKey),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat))
{
    ui->setupUi(this);
    //init UI
    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_privkey_input->setText(mKeyring.getPrivateKey()->toBase64());
    ui->rsa_pubkey_input->setText(mKeyring.getPublicKey()->toBase64());
    ui->rsa_generate_pb->setMaximumWidth(ui->rsa_generate_bt->width());
    ui->rsa_generate_pb->setVisible(false);

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    connect(ui->save_bt, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel_bt, SIGNAL(clicked()),
            this, SLOT(cancel()));

    connect(ui->rsa_pubkey_input, SIGNAL(textChanged()),
            this, SLOT(changeRsaPubKey()));
    connect(ui->rsa_privkey_input, SIGNAL(textChanged()),
            this, SLOT(changeRsaPrivKey()));
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

void SettingsWindow::changeRsaPubKey(){
    QByteArray inputPubKey = ui->rsa_pubkey_input->toPlainText().toUtf8();
    mKeyring.setPublicKey(QByteArray::fromBase64(inputPubKey));
}

void SettingsWindow::changeRsaPrivKey(){
    QByteArray inputPrivKey = ui->rsa_privkey_input->toPlainText().toUtf8();
    mKeyring.setPrivateKey(QByteArray::fromBase64(inputPrivKey));
}

void SettingsWindow::rsaExport(){
    QString filename = QFileDialog::getSaveFileName(this, "Save Key", "",
                                                          "Key files (*.key *.pem);;Other files (*.*)");
    mKeyring.exportKeys(filename);
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
    mKeyring.importKeys(filename);
}

void SettingsWindow::save(){
    //TODO: set default values
    mSettings->setValue("network/listen_port", ui->net_port_input->text());
    mKeyring.commitToKeystore();
    emit settingsUpdated();
    close();
    deleteLater();
}

SettingsWindow::~SettingsWindow(){
    delete ui, mSettings;
}
