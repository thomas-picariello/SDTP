#ifndef CONFWIZARD_H
#define CONFWIZARD_H

#include <QDialog>

namespace Ui {
class ConfWizard;
}

class ConfWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ConfWizard(QWidget *parent = 0);
    ~ConfWizard();

private slots:
    void on_KeyButton_clicked();

    void on_NetworkButton_clicked();

    void on_ContactsButton_clicked();

    void on_PasswordButton_clicked();

private:
    Ui::ConfWizard *ui;
};

#endif // CONFWIZARD_H
