#ifndef PASSWORDWINDOW_H
#define PASSWORDWINDOW_H

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>

namespace Ui {
class PasswordWindow;
}

class PasswordWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordWindow(QByteArray pwdHash, QByteArray salt, QWidget *parent = 0);
    ~PasswordWindow();

signals:
    void error(QString err);
    void validate(QByteArray key);

public slots:
    void onOkClick();

private:
    QByteArray mPwdHash, mSalt;
    Ui::PasswordWindow *ui;

    QByteArray deriveKey(QString &password);
};

#endif // PASSWORDWINDOW_H
