#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QPair<QByteArray, QByteArray> *fileKey, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mFileKey(fileKey),
    mKeyring(mFileKey),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat,this))
{
    ui->setupUi(this);
    //init UI
    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_privkey_input->setText(mKeyring.getPrivateKey().toBase64());
    ui->rsa_pubkey_input->setText(mKeyring.getPublicKey().toBase64());

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    setupRsaMenus();

    connect(ui->save_bt, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel_bt, SIGNAL(clicked()),
            this, SLOT(cancel()));

    connect(ui->rsa_privkey_input, SIGNAL(textChanged()),
            this, SLOT(changeRsaPrivKey()));
    connect(ui->rsa_pubkey_opt_bt, SIGNAL(clicked()),
            this, SLOT(showRsaPubkeyMenu()));
    connect(ui->rsa_privkey_opt_bt, SIGNAL(clicked()),
            this, SLOT(showRsaPrivkeyMenu()));
    connect(&mKeyring, SIGNAL(keyGenerationFinished()),
            this, SLOT(rsaKeyGenFinished()));
    connect(&mGeneratingAnimTimer, SIGNAL(timeout()),
            this, SLOT(rsaGenerateAnimate()));
}

void SettingsWindow::cancel(){
    hide();
}

void SettingsWindow::changeRsaPrivKey(){
    QByteArray inputPrivKey = ui->rsa_privkey_input->toPlainText().toUtf8();
    mKeyring.setPrivateKey(QByteArray::fromBase64(inputPrivKey));
}

void SettingsWindow::rsaExportPrivate(){
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Private Key"),
                                                    QDir::home().absoluteFilePath(tr("private_key.key")),
                                                    tr("Key file (*.key)"));
    if(!filename.isNull())
        mKeyring.exportPrivateKey(filename);
}

void SettingsWindow::rsaExportPublic(){
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Public Key"),
                                                    QDir::home().absoluteFilePath(tr("public_key.key")),
                                                    tr("Key file")+" (*.key)");
    if(!filename.isNull())
        mKeyring.exportPublicKey(filename);
}

void SettingsWindow::rsaGenerateKeypair(){
    if(!mKeyring.isGenerating()){
        mGeneratingAnimTimer.start(100);
        mKeyring.generateKeypair();
    }
}

void SettingsWindow::rsaKeyGenFinished(){
    mGeneratingAnimTimer.stop();
    ui->rsa_privkey_input->setText(mKeyring.getPrivateKey().toBase64());
    ui->rsa_pubkey_input->setText(mKeyring.getPublicKey().toBase64());
}

void SettingsWindow::rsaImportPrivate(){
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Import private key"),
                                                    QDir::home().absolutePath(),
                                                    tr("Key file")+" (*.key)");
    if(!filename.isNull()){
        mKeyring.importPrivateKey(filename);
        ui->rsa_privkey_input->setText(mKeyring.getPrivateKey().toBase64());
    }
}

void SettingsWindow::rsaGeneratePublic(){
    if(!mKeyring.getPrivateKey().isEmpty()){
        mKeyring.generatePublicKey();
        ui->rsa_pubkey_input->setText(mKeyring.getPublicKey().toBase64());
    }else{
        //TODO: show error
    }
}

void SettingsWindow::rsaGenerateAnimate(){
    if(ui->rsa_privkey_input->toPlainText().length() > 3){
        ui->rsa_pubkey_input->clear();
        ui->rsa_privkey_input->clear();
    }
    ui->rsa_pubkey_input->setText(ui->rsa_pubkey_input->toPlainText()+"|");
    ui->rsa_privkey_input->setText(ui->rsa_privkey_input->toPlainText()+"|");
}

void SettingsWindow::save(){
    QString input = ui->net_port_input->text();
    if(input.isEmpty())
        input = "8000"; //default listen port number
    mSettings->setValue("network/listen_port", input);
    mKeyring.commitToKeystore();
    emit settingsUpdated();
    hide();
}

void SettingsWindow::setupRsaMenus(){
    mRsaPubkeyMenu = new QMenu(ui->rsa_pubkey_opt_bt);
    mRsaPubkeyMenu->addAction(tr("Export public key"),
                              this, SLOT(rsaExportPublic()));
    mRsaPubkeyMenu->addAction(tr("Generate public key from private key"),
                              this, SLOT(rsaGeneratePublic()));

    mRsaPrivkeyMenu = new QMenu(ui->rsa_privkey_opt_bt);
    mRsaPrivkeyMenu->addAction(tr("Export private key"),
                               this, SLOT(rsaExportPrivate()));
    mRsaPrivkeyMenu->addAction(tr("Import private key"),
                               this, SLOT(rsaImportPrivate()));
    mRsaPrivkeyMenu->addAction(tr("Generate Keypair"),
                               this, SLOT(rsaGenerateKeypair()));
}

void SettingsWindow::showRsaPubkeyMenu(){
    QPoint pos = ui->rsa_pubkey_opt_bt->geometry().bottomLeft();
    pos = ui->rsa_pubkey_opt_bt->mapFromParent(pos);
    pos = ui->rsa_pubkey_opt_bt->mapToGlobal(pos);
    mRsaPubkeyMenu->exec(pos);
}

void SettingsWindow::showRsaPrivkeyMenu(){
    QPoint pos = ui->rsa_privkey_opt_bt->geometry().bottomLeft();
    pos = ui->rsa_privkey_opt_bt->mapFromParent(pos);
    pos = ui->rsa_privkey_opt_bt->mapToGlobal(pos);
    mRsaPrivkeyMenu->exec(pos);
}

SettingsWindow::~SettingsWindow(){
    delete ui;
}
