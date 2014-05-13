#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>
#include <QFileDialog>
#include "rsakeyring.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QPair<QByteArray,QByteArray> *fileKey, QWidget *parent = 0);
    ~SettingsWindow();

signals:
    void settingsUpdated();
    void error(QString err);

public slots:
    void cancel();
    void changeRsaPrivKey();
    void rsaExportPrivate();
    void rsaExportPublic();
    void rsaGenerateKeypair();
    void rsaKeyGenFinished();
    void rsaImportPrivate();
    void rsaGeneratePublic();
    void showRsaPubkeyMenu();
    void showRsaPrivkeyMenu();
    void save();

private:
    Ui::SettingsWindow *ui;
    QSettings *mSettings;
    QPair<QByteArray,QByteArray> *mFileKey;
    QIntValidator mPortValidator;
    RsaKeyring mKeyring;

    QMenu *mRsaPubkeyMenu;
    QMenu *mRsaPrivkeyMenu;

    void setupRsaMenus();
};

#endif // SETTINGSWINDOW_H
