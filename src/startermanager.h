#ifndef StarterManager_H
#define StarterManager_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QLayout>
#include <QSettings>
#include <QMessageBox>

#include "starter.h"



class StarterManager: public QWidget
{

     Q_OBJECT

public:
    StarterManager();
    ~StarterManager();

public slots :
    void starterWindow();
    void verifyContact();

private :
    QWidget  *namewindow;
    QPushButton *okay, *cancel;
    QLineEdit *name;
    QVBoxLayout *namelayout;
    QHBoxLayout *buttonlayout;
    QSettings *settings;

    Starter *starter;






};

#endif // StarterManager_H
