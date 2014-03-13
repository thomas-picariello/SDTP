#include "passwordwindow.h"
#include "ui_passwordwindow.h"

PasswordWindow::PasswordWindow(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint),
    ui(new Ui::PasswordWindow)
{
    ui->setupUi(this);

    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->ok, SIGNAL(clicked()),
            this, SLOT(onOkClick()));
}

void PasswordWindow::onOkClick(){
    emit validate(ui->password->text());
    ui->password->clear();
}

PasswordWindow::~PasswordWindow(){
    delete ui;
}
