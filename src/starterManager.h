#ifndef STARTER_H
#define STARTER_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>

class Starter: public QWidget
{

     Q_OBJECT

public:
    Starter();
    ~Starter();

public slots :
    void starterWindow();

private :
    QWidget *starterwindow;
    QPushButton *about;


};

#endif // STARTER_H
