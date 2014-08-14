#ifndef CONFWIZARD_H
#define CONFWIZARD_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
//#include <QDebug>
#include <QPair>
#include <QSettings>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>
#include "rsakeyring.h"
namespace Ui {
class ConfWizard;
}

/**
 * @brief ConfWizard is an Object which helps to configure the main settings on the first launch of the app.
 */
class ConfWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ConfWizard(QPair<QByteArray,QByteArray> *fileKey, RsaKeyring *rsaKeyring, QWidget *parent = 0);
    ~ConfWizard();
    bool verifyPort();
    bool verifyPass();


private slots:
    void on_okButton_clicked();
    void on_GenerateButton_clicked();
    void on_PrivateKey_Generated(QByteArray pkey);
    void saveAll();

private:
    Ui::ConfWizard *ui;
    QPair<QByteArray,QByteArray> *m_fileKey;
    RsaKeyring *m_RsaKeyring;
    QSettings *m_Settings;
    QMessageBox *m_MsgBox, *m_WarningBox;
};

#endif // CONFWIZARD_H
