#ifndef PASSWORDWINDOW_H
#define PASSWORDWINDOW_H

#include <QDialog>

namespace Ui {
class PasswordWindow;
}

class PasswordWindow : public QDialog
{
    Q_OBJECT

signals:
    void validate(QString password);

public slots:
    void onOkClick();

public:
    explicit PasswordWindow(QWidget *parent = 0);
    ~PasswordWindow();

private:
    Ui::PasswordWindow *ui;
};

#endif // PASSWORDWINDOW_H
