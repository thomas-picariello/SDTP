#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QPair<QByteArray, QByteArray> *fileKey, RsaKeyring *keyring, ContactDB *contactDB, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    mContactDB(contactDB),
    mFileKey(fileKey),
    mKeyring(keyring),
    mSettings(new QSettings("settings.ini", QSettings::IniFormat,this))
{
    ui->setupUi(this);

    mPortValidator.setRange(0, 65535);
    ui->net_port_input->setValidator(&mPortValidator);

    ui->rsa_privkey_input->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->rsa_pubkey_input->setWordWrapMode(QTextOption::WrapAnywhere);

    setupRsaMenus();

    connect(ui->save_bt, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancel_bt, SIGNAL(clicked()),
            this, SLOT(cancel()));

    connect(ui->rsa_pubkey_opt_bt, SIGNAL(clicked()),
            this, SLOT(showRsaPubkeyMenu()));
    connect(ui->rsa_privkey_opt_bt, SIGNAL(clicked()),
            this, SLOT(showRsaPrivkeyMenu()));
    connect(ui->pwd_cleartxt_checkbox, SIGNAL(toggled(bool)),
            this, SLOT(pwdSetVisible(bool)));
    connect(ui->pwd_actualpwd_input, SIGNAL(textChanged(QString)),
            this, SLOT(pwdValidateOld()));
    connect(ui->pwd_newpwd_input, SIGNAL(textChanged(QString)),
            this, SLOT(pwdTestMatch()));
    connect(ui->pwd_confpwd_input, SIGNAL(textChanged(QString)),
            this, SLOT(pwdTestMatch()));

    connect(mKeyring, SIGNAL(privateKeyGenerationFinished(QByteArray)),
            this, SLOT(rsaPrivateKeyGenFinished(QByteArray)));
    connect(mKeyring, SIGNAL(publicKeyGenerationFinished(QByteArray)),
            this, SLOT(rsaPublicKeyGenFinished(QByteArray)));
    connect(&mGeneratingAnimTimer, SIGNAL(timeout()),
            this, SLOT(rsaGenerateAnimate()));
}

void SettingsWindow::cancel(){
    hide();
}

void SettingsWindow::pwdSetVisible(bool visible){
    if(visible){
        ui->pwd_actualpwd_input->setEchoMode(QLineEdit::Normal);
        ui->pwd_newpwd_input->setEchoMode(QLineEdit::Normal);
        ui->pwd_confpwd_input->setEchoMode(QLineEdit::Normal);
    }else{
        ui->pwd_actualpwd_input->setEchoMode(QLineEdit::Password);
        ui->pwd_newpwd_input->setEchoMode(QLineEdit::Password);
        ui->pwd_confpwd_input->setEchoMode(QLineEdit::Password);
    }
}

bool SettingsWindow::pwdValidateOld(){
    QByteArray storedHash = QByteArray::fromBase64(mSettings->value("encryption/password_hash").toByteArray());
    if(!storedHash.isEmpty()){
        QByteArray currentHash = hashPassword(ui->pwd_actualpwd_input->text());
        if(storedHash == currentHash){
            ui->pwd_actualpwd_input->setStyleSheet("border: 1px solid #0a0");
            return true;
        }else{
            ui->pwd_actualpwd_input->setStyleSheet("");
            return false;
        }
    }else{
        return true;
    }
}

bool SettingsWindow::pwdTestMatch(){
    QString newPwd = ui->pwd_newpwd_input->text();
    QString confPwd = ui->pwd_confpwd_input->text();
    if(!newPwd.isEmpty() && !confPwd.isEmpty()){
        if(confPwd.compare(newPwd) == 0){
            ui->pwd_newpwd_input->setStyleSheet("border: 1px solid #0a0");
            ui->pwd_confpwd_input->setStyleSheet("border: 1px solid #0a0");
            return true;
        }else{
            ui->pwd_newpwd_input->setStyleSheet("border: 1px solid #a00");
            ui->pwd_confpwd_input->setStyleSheet("border: 1px solid #a00");
            return false;
        }
    }else{
        ui->pwd_newpwd_input->setStyleSheet("");
        ui->pwd_confpwd_input->setStyleSheet("");
        return true;    //match if empty to allow to remove pwd
    }
}

void SettingsWindow::rsaExportPrivate(){
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Private Key"),
                                                    QDir::home().absoluteFilePath(tr("private_key.key")),
                                                    tr("Key file (*.key)"));
    if(!filename.isNull()){
        QFile file(filename);
        file.open(QFile::WriteOnly);
        if(file.isOpen()){
            QByteArray privateKey = QByteArray::fromBase64(ui->rsa_privkey_input->toPlainText().toUtf8());
            file.write(privateKey);
            file.close();
        }else{
            emit error("bad filename"); //TODO: enum
        }
    }
}

void SettingsWindow::rsaExportPublic(){
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Public Key"),
                                                    QDir::home().absoluteFilePath(tr("public_key.key")),
                                                    tr("Key file")+" (*.key)");
    if(!filename.isNull()){
        QFile file(filename);
        file.open(QFile::WriteOnly);
        if(file.isOpen()){
            QByteArray publicKey = QByteArray::fromBase64(ui->rsa_pubkey_input->toPlainText().toUtf8());
            file.write(publicKey);
            file.close();
        }else{
            emit error("bad filename"); //TODO: enum
        }
    }
}

void SettingsWindow::rsaGenerateKeypair(){
    if(!mKeyring->isGenerating()){
        mGeneratingAnimTimer.start(100);
        mKeyring->generateKeypair();
    }
}

void SettingsWindow::rsaPrivateKeyGenFinished(QByteArray privateKey){
    mGeneratingAnimTimer.stop();
    ui->rsa_privkey_input->setText(privateKey.toBase64());
}

void SettingsWindow::rsaPublicKeyGenFinished(QByteArray publicKey){
    ui->rsa_pubkey_input->setText(publicKey.toBase64());
}

void SettingsWindow::rsaImportPrivate(){
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Import private key"),
                                                    QDir::home().absolutePath(),
                                                    tr("Key file")+" (*.key)");
    if(!filename.isNull()){
        QFile file(filename);
        file.open(QFile::ReadOnly);
        if(file.isOpen()){
            QByteArray privateKey = file.readAll();
            if(mKeyring->validatePrivateKey(privateKey)){
                ui->rsa_privkey_input->setText(privateKey.toBase64());
                rsaGeneratePublicKey();
            }else{
                emit error("bad key"); //TODO: enum
            }
        }
    }
}

void SettingsWindow::rsaGeneratePublicKey(){
    QByteArray privateKey = QByteArray::fromBase64(ui->rsa_privkey_input->toPlainText().toUtf8());
    mKeyring->generatePublicKey(privateKey);
}

void SettingsWindow::rsaGenerateAnimate(){
    if(ui->rsa_privkey_input->toPlainText().length() > 3){
        ui->rsa_pubkey_input->clear();
        ui->rsa_privkey_input->clear();
    }else{
        ui->rsa_pubkey_input->setText(ui->rsa_pubkey_input->toPlainText()+"|");
        ui->rsa_privkey_input->setText(ui->rsa_privkey_input->toPlainText()+"|");
    }
}

void SettingsWindow::save(){
    //save password
    if(!ui->pwd_actualpwd_input->text().isEmpty() ||
       !ui->pwd_newpwd_input->text().isEmpty()    ||
       !ui->pwd_confpwd_input->text().isEmpty())
    {
        if(pwdValidateOld() && pwdTestMatch()){
            QString password = ui->pwd_newpwd_input->text();
            QByteArray oldKey = mFileKey->first;
            if(password.isEmpty()){
                //TODO: confirmation
                mSettings->remove("encryption/password_hash");
                mFileKey->first.clear();
            }else{
                mSettings->setValue("encryption/password_hash", hashPassword(password).toBase64());
                mFileKey->first = deriveKey(password);
            }
            mKeyring->updateFileKey(oldKey);
            mContactDB->updateFileKey();
        }else{
            emit error(tr("Bad password"));
        }
    }

    //save port
    QString port = ui->net_port_input->text();
    if(port.isEmpty())
        port = "8000"; //default listen port number
    mSettings->setValue("network/listen_port", port);

    //save private key
    QByteArray privateKey = QByteArray::fromBase64(ui->rsa_privkey_input->toPlainText().toUtf8());
    if(privateKey != mKeyring->getStoredPrivateKey()){
        if(mKeyring->validatePrivateKey(privateKey))
            mKeyring->commitToKeystore(privateKey);
        else
            emit error("Bad key"); //TODO: enum
    }
    emit settingsUpdated();
    hide();
}

void SettingsWindow::hideEvent(QHideEvent *){
    ui->pwd_actualpwd_input->clear();
    ui->pwd_newpwd_input->clear();
    ui->pwd_confpwd_input->clear();
}

void SettingsWindow::showEvent(QShowEvent*){
    if(mSettings->value("encryption/password_hash").toByteArray().isEmpty()){
        ui->pwd_actualpwd_input->setDisabled(true);
        ui->pwd_actualpwd_input->setPlaceholderText(tr("No actual password"));
    }else{
        ui->pwd_actualpwd_input->setDisabled(false);
        ui->pwd_actualpwd_input->setPlaceholderText(tr("Enter your actual password"));
    }
    ui->net_port_input->setText(mSettings->value("network/listen_port").toString());
    ui->rsa_privkey_input->setText(mKeyring->getStoredPrivateKey().toBase64());
    mKeyring->generatePublicKey(mKeyring->getStoredPrivateKey());
}

QByteArray SettingsWindow::hashPassword(QString &password){
    QByteArray salt = QByteArray::fromBase64(mSettings->value("encryption/salt").toByteArray());
    std::string digest;
    CryptoPP::SHA256 hash;
    password.append(salt);
    CryptoPP::StringSource(password.toStdString(),
                           true,
                           new CryptoPP::HashFilter(hash,
                                new CryptoPP::StringSink(digest)
                                    )); //no new line
    return QByteArray(digest.data(), (int)digest.size());
}

QByteArray SettingsWindow::deriveKey(QString &password){
    QByteArray salt = QByteArray::fromBase64(mSettings->value("encryption/salt").toByteArray());
    CryptoPP::PKCS5_PBKDF1<CryptoPP::SHA256> derivator;
    uchar key[32];
    derivator.DeriveKey(key,
                        derivator.MaxDerivedKeyLength(),
                        0,
                        reinterpret_cast<const uchar*>(password.toUtf8().constData()),
                        password.length(),
                        reinterpret_cast<const uchar*>(salt.constData()),
                        salt.length(),
                        1000, //iteration number
                        0);
    return QByteArray(reinterpret_cast<char*>(key), (uint)derivator.MaxDerivedKeyLength());
}

void SettingsWindow::setupRsaMenus(){
    mRsaPubkeyMenu = new QMenu(ui->rsa_pubkey_opt_bt);
    mRsaPubkeyMenu->addAction(tr("Export public key"),
                              this, SLOT(rsaExportPublic()));
    mRsaPubkeyMenu->addAction(tr("Generate public key from private key"),
                              this, SLOT(rsaGeneratePublicKey()));

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
