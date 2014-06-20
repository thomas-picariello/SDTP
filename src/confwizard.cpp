#include "confwizard.h"
#include "ui_confwizard.h"

ConfWizard::ConfWizard(QPair<QByteArray, QByteArray> fileKey, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfWizard)
{
    ui->setupUi(this);

    m_RsaKeyRing = new RsaKeyring(&fileKey);

    m_fileKey = new QPair<QByteArray, QByteArray>;
    m_fileKey->first = fileKey.first;
    m_fileKey->second = fileKey.second;


    QFile file(("ReadMe.txt"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);


        QString WelcomeTxt = file.readAll();


    m_MsgBox = new QMessageBox(QMessageBox::NoIcon,
                               "Welcome stranger",
                               WelcomeTxt,
                               QMessageBox::Ok);
    m_WarningBox = new QMessageBox(QMessageBox::Warning,
                                   "Warning",
                                   "One of the parameters seems to be wrong.",
                                   QMessageBox::Close);

    m_MsgBox->show();


    connect(m_MsgBox,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(show()));

}
bool ConfWizard::verifyPass(){
    QString pass1 = ui->PassLine->text(),pass2 = ui->ConfirmLine->text();

    if(pass1.isEmpty() && pass2.isEmpty())return true;
    else if (pass1.compare(pass2)==0)return true;
    else return false;

}
bool ConfWizard::verifyPort(){

    qDebug()<<ui->portBox->value();
    if(ui->portBox->value()<=65000 && ui->portBox->value() >= 1)return true;
    else return false;
}
void ConfWizard::saveAll(){



    disconnect(m_RsaKeyRing,SIGNAL(privateKeyValidated()),this,SLOT(saveAll()));

    // save port
    m_Settings = new QSettings("settings.ini", QSettings::IniFormat,this);
    m_Settings->setValue("network/listen_port", ui->portBox->text());

    // save psw
    QString pass1 = ui->PassLine->text(),pass2 = ui->ConfirmLine->text();
    if( !pass1.isEmpty() && !pass2.isEmpty() ){

        QByteArray salt = QByteArray::fromBase64(m_Settings->value("encryption/salt").toByteArray());
        std::string digest;
        pass1.append(salt);
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource(pass1.toStdString(),
                               true,
                               new CryptoPP::HashFilter(hash,
                                    new CryptoPP::StringSink(digest)
                                        )); //no new line

        m_Settings->setValue("encryption/password_hash", QByteArray(digest.data(), (int)digest.size()).toBase64());

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


    m_RsaKeyRing = new RsaKeyring(m_fileKey);
        m_RsaKeyRing->commitToKeystore(privateKey);



    this->accept();

}


void ConfWizard::on_okButton_clicked()
{
    if(verifyPass() && verifyPort()){
        QByteArray key = QByteArray::fromBase64(ui->keyText->toPlainText().toUtf8());
        connect(m_RsaKeyRing,SIGNAL(privateKeyValidated()),this,SLOT(saveAll()));
        m_RsaKeyRing->validatePrivateKey(key);
    }else m_WarningBox->show();
}

void ConfWizard::on_GenerateButton_clicked()
{
    if(!m_RsaKeyRing->isGenerating()){
        connect(m_RsaKeyRing,SIGNAL(privateKeyGenerationFinished(QByteArray)),
                this,SLOT(on_PrivateKey_Generated(QByteArray)));
        m_RsaKeyRing->generateKeypair();
    }


}
void ConfWizard::on_PrivateKey_Generated(QByteArray pkey){
    ui->keyText->setText(pkey.toBase64());
    disconnect(m_RsaKeyRing,SIGNAL(privateKeyGenerationFinished(QByteArray)),
               this,SLOT(on_PrivateKey_Generated(QByteArray)));
}

ConfWizard::~ConfWizard()
{
    delete ui;
}
