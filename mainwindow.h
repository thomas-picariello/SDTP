#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    QPushButton *boutonStart, *boutonContacts, *boutonSettings;
    QHBoxLayout *layout;
    QWidget *window;
    QAction *Start, *Contacts, *Settings;
};

#endif // MAINWINDOW_H
