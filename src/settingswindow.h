#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
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

public slots:
    void cancel();
    void changeRsaPubKey();
    void changeRsaPrivKey();
    void rsaExport();
    void rsaGenerate();
    void rsaKeyGenFinished();
    void rsaImport();
    void save();

private:
    Ui::SettingsWindow *ui;
    QSettings *mSettings;
    QPair<QByteArray,QByteArray> *mFileKey;
    QIntValidator mPortValidator;
    RsaKeyring mKeyring;
};

#endif // SETTINGSWINDOW_H
