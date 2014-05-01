#include "passwordwindow.h"
#include "ui_passwordwindow.h"

PasswordWindow::PasswordWindow(QString pwdHash, QString salt, QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint),
    ui(new Ui::PasswordWindow),
    mPwdHash(pwdHash),
    mSalt(salt)
{
    ui->setupUi(this);

    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->ok, SIGNAL(clicked()),
            this, SLOT(onOkClick()));
    show();
}

void PasswordWindow::onOkClick(){
    if(!ui->password->text().isEmpty()){
        QString password = ui->password->text();
        std::string digest;
        CryptoPP::SHA256 hash;
        password.append(QByteArray::fromBase64(mSalt.toUtf8()));
        CryptoPP::StringSource(password.toStdString(),
                               true,
                               new CryptoPP::HashFilter(hash,
                                    new CryptoPP::Base64Encoder(
                                        new CryptoPP::StringSink(digest),
                                        false))); //no new line
        qDebug()<<"Hash from entered password"<<QString::fromStdString(digest);
        if(mPwdHash.compare(QString::fromStdString(digest)) != 0){
            emit error("Error: Password hashes do not match.");
            QMessageBox::critical(this, "Error", "Wrong password !");
        }else{
            emit validate(deriveKey(password));
            close();
        }
        ui->password->clear();
    }
}

QByteArray PasswordWindow::deriveKey(QString &password){
    CryptoPP::PKCS5_PBKDF1<CryptoPP::SHA256> derivator;
    uchar key[32];
    derivator.DeriveKey(key,
                        derivator.MaxDerivedKeyLength(),
                        0,
                        reinterpret_cast<const uchar*>(password.toUtf8().constData()),
                        password.length(),
                        reinterpret_cast<const uchar*>(mSalt.toUtf8().constData()),
                        mSalt.length(),
                        1000, //iteration number
                        0);
    qDebug()<<"Gen AES key"
            <<QByteArray(reinterpret_cast<char*>(key), static_cast<uint>(derivator.MaxDerivedKeyLength())).toBase64();
    return QByteArray(reinterpret_cast<char*>(key), static_cast<uint>(derivator.MaxDerivedKeyLength()));
}

PasswordWindow::~PasswordWindow(){
    delete ui;
}
