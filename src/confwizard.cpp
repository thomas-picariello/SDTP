#include "confwizard.h"
#include "ui_confwizard.h"

ConfWizard::ConfWizard(QPair<QByteArray, QByteArray>* fileKey, RsaKeyring* rsaKeyring, QWidget *parent) :
    QDialog(parent),
    m_welcomeMsgBox(NULL),
    m_warningMsgBox(NULL),
    ui(new Ui::ConfWizard)
{
    ui->setupUi(this);

    m_fileKey = fileKey;
    m_rsaKeyring = rsaKeyring;

    QFile file("ReadMe.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen()){
        QString WelcomeTxt = file.readAll();
        file.close();
        m_welcomeMsgBox = new QMessageBox(QMessageBox::NoIcon,
                                   "Welcome",
                                   WelcomeTxt,
                                   QMessageBox::Ok);
        m_welcomeMsgBox->show();
        connect(m_welcomeMsgBox,&QMessageBox::buttonClicked,
                this, &ConfWizard::show);
    }else{
        show();
    }
    m_warningMsgBox = new QMessageBox(QMessageBox::Warning,
                                   "Warning",
                                   "One of the parameters seems to be wrong.",
                                   QMessageBox::Close);
}
bool ConfWizard::verifyPass(){
    QString pass1 = ui->PassLine->text(),pass2 = ui->ConfirmLine->text();

    if(pass1.isEmpty() && pass2.isEmpty())return true;
    else if (pass1.compare(pass2)==0)return true;
    else return false;

}
bool ConfWizard::verifyPort(){

    //qDebug()<<ui->portBox->value();
    if(ui->portBox->value()<=65000 && ui->portBox->value() >= 1)return true;
    else return false;
}
void ConfWizard::saveAll(){
    disconnect(m_rsaKeyring,SIGNAL(privateKeyValidated()),this,SLOT(saveAll()));

    // save port
    QSettings settings("settings.ini", QSettings::IniFormat, this);
    settings.setValue("network/listen_port", ui->portBox->text());

    QByteArray oldkey = m_fileKey->first;
    // save psw
    QString pass1 = ui->PassLine->text(),pass2 = ui->ConfirmLine->text();
    if( !pass1.isEmpty() && !pass2.isEmpty() ){

        QByteArray salt = QByteArray::fromBase64(settings.value("encryption/salt").toByteArray());
        std::string digest;
        pass1.append(salt);
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource(pass1.toStdString(),
                               true,
                               new CryptoPP::HashFilter(hash,
                                    new CryptoPP::StringSink(digest)
                                        )); //no new line

        settings.setValue("encryption/password_hash", QByteArray(digest.data(), (int)digest.size()).toBase64());

        CryptoPP::PKCS5_PBKDF1<CryptoPP::SHA256> derivator;
        uchar key[32];

        derivator.DeriveKey(key,
                            derivator.MaxDerivedKeyLength(),
                            0,
                            reinterpret_cast<const uchar*>(pass1.toUtf8().constData()),
                            pass1.length(),
                            reinterpret_cast<const uchar*>(salt.constData()),
                            salt.length(),
                            1000, //iteration number
                            0);

        m_fileKey->first = QByteArray(reinterpret_cast<char*>(key), (uint)derivator.MaxDerivedKeyLength());
    }

    //save rsakey
    QByteArray privateKey = QByteArray::fromBase64(ui->keyText->toPlainText().toUtf8());

    m_rsaKeyring->updateFileKey(oldkey);
    if(m_rsaKeyring->validatePrivateKey(privateKey))
        m_rsaKeyring->commitToKeystore(privateKey);

    this->accept();
}

void ConfWizard::on_okButton_clicked(){
    if(verifyPass() && verifyPort()){
        QByteArray key = QByteArray::fromBase64(ui->keyText->toPlainText().toUtf8());
        connect(m_rsaKeyring,SIGNAL(privateKeyValidated()),this,SLOT(saveAll()));
        m_rsaKeyring->validatePrivateKey(key);
        //TODO: connect(m_RsaKeyring, SIGNAL(error(RsaKeyring::error)), this, ...); for Error::PrivateKeyValidationFailed
    }else m_warningMsgBox->show();
}

void ConfWizard::on_GenerateButton_clicked(){
    if(!m_rsaKeyring->isGenerating()){
        connect(m_rsaKeyring,SIGNAL(privateKeyGenerationFinished(QByteArray)),
                this,SLOT(onPrivateKeyGenerated(QByteArray)));
        m_rsaKeyring->generateKeypair();
    }
}
void ConfWizard::onPrivateKeyGenerated(QByteArray pkey){
    ui->keyText->setText(pkey.toBase64());
    disconnect(m_rsaKeyring,SIGNAL(privateKeyGenerationFinished(QByteArray)),
               this,SLOT(onPrivateKeyGenerated(QByteArray)));
}

ConfWizard::~ConfWizard()
{
    delete ui;
    if(m_welcomeMsgBox)
        delete m_welcomeMsgBox;
    delete m_warningMsgBox;
}
