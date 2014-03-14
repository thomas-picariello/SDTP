#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>
#include "rsakeyring.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QByteArray *fileKey, QWidget *parent = 0);
    ~SettingsWindow();

signals:
    void settingsUpdated();

public slots:
    void cancel();
    void rsaExport();
    void rsaGenerate();
    void rsaImport();
    void save();

private:
    Ui::SettingsWindow *ui;
    QSettings *mSettings;
    QByteArray *mFileKey;
    QIntValidator mPortValidator;
    RsaKeyring mKeyring;
};

#endif // SETTINGSWINDOW_H
