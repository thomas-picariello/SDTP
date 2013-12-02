#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>

class Settings : public QWidget
{

    Q_OBJECT

public:
    Settings();
    ~Settings();

public slots :
    void settingsWindow();

private :
    QWidget *setwindow;
    QPushButton *about;

};

#endif // SETTINGS_H
