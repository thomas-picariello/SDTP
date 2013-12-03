#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>

class Settings : public QWidget
{

    Q_OBJECT

public:
    Settings();
    ~Settings();

public slots :
    void settingsWindow();
    void savesettings();

private :
    QWidget *setwindow;
    QVBoxLayout *labellayout, *linelayout;
    QHBoxLayout *slayout;
    QLabel *portlabel, *keylabel;
    QLineEdit *portline, *keyline;
    QPushButton *save, *close;
    QSettings *settings;
    //QPushButton *about;

};

#endif // SETTINGS_H
