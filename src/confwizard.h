#ifndef CONFWIZARD_H
#define CONFWIZARD_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QPair>
#include "rsakeyring.h"
namespace Ui {
class ConfWizard;
}

class ConfWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ConfWizard(QPair<QByteArray,QByteArray> fileKey,QWidget *parent = 0);
    ~ConfWizard();
    bool verifyPort();
    bool verifyPass();
    bool verifyKey();
    bool saveAll();

private slots:
    void on_okButton_clicked();
    void on_GenerateButton_clicked();

private:
    Ui::ConfWizard *ui;
    QPair<QByteArray,QByteArray> *m_fileKey;
    RsaKeyring *m_RsaKeyRing;

    QMessageBox *m_MsgBox, *m_WarningBox;
};

#endif // CONFWIZARD_H
