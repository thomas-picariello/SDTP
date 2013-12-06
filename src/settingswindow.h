#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

public slots:
    void save();
    void cancel();
    
private:
    Ui::SettingsWindow *ui;
    QSettings mSettings;
};

#endif // SETTINGSWINDOW_H
