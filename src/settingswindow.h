#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>
#include <QFileDialog>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>
#include "rsakeyring.h"
#include "contactdb.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QPair<QByteArray,QByteArray> *fileKey,
                            RsaKeyring *keyring,
                            ContactDB *contactDB,
                            QWidget *parent = 0);
    ~SettingsWindow();

signals:
    void settingsUpdated();
    void error(QString err);

private slots:
    void cancel();
    bool pwdTestMatch();
    void pwdSetVisible(bool visible);
    bool pwdValidateOld();
    void rsaExportPrivate();
    void rsaExportPublic();
    void rsaGenerateKeypair();
    void rsaPrivateKeyGenFinished(QByteArray privateKey);
    void rsaPublicKeyGenFinished(QByteArray publicKey);
    void rsaImportPrivate();
    void rsaGeneratePublicKey();
    void rsaGenerateAnimate();
    void showRsaPubkeyMenu();
    void showRsaPrivkeyMenu();
    void save();

protected:
    void showEvent(QShowEvent*);
    void hideEvent(QHideEvent*);

private:
    Ui::SettingsWindow *ui;
    QSettings *mSettings;
    QPair<QByteArray,QByteArray> *mFileKey;
    ContactDB *mContactDB;
    QIntValidator mPortValidator;
    RsaKeyring *mKeyring;

    QMenu *mRsaPubkeyMenu;
    QMenu *mRsaPrivkeyMenu;
    QTimer mGeneratingAnimTimer;

    QByteArray hashPassword(QString &password);
    QByteArray deriveKey(QString &password);
    void setupRsaMenus();
};

#endif // SETTINGSWINDOW_H
