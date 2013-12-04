#include "starterManager.h"

StarterManager::StarterManager()
{
    namewindow = new QWidget;

    okay = new QPushButton;
    cancel = new QPushButton;

    name = new QLineEdit;

    namelayout = new QVBoxLayout;
    buttonlayout = new QHBoxLayout;

    settings = new QSettings;




    okay->setText("Okay");
    cancel->setText("Cancel");

    name->setInputMask("nnnnnnnnnn");
    name->setMaxLength(10);
    name->setPlaceholderText("Contact name :");
    name->setText("Default");

    buttonlayout->addWidget(okay);
    buttonlayout->addWidget(cancel);

    namelayout->addWidget(name);
    namelayout->addLayout(buttonlayout);

    namewindow->setLayout(namelayout);

    connect(cancel, SIGNAL(clicked()),namewindow,SLOT(hide()));
    connect(okay,SIGNAL(clicked()),this,SLOT(verifyContact()));




}
void StarterManager::starterWindow()
{
    qDebug()<<"Test for namewindow";
    namewindow->show();
}
void StarterManager::verifyContact()
{
    settings->beginGroup("Contacts/");
    QStringList list;
    list = settings->childGroups();

    qDebug()<<"Contenu de la variable 'list'(childgroup) |"+list.join(" : ");
    qDebug()<<"Contenu de la variable 'name'(qlineedit)  |"+name->text();

    if(list.contains(name->text()))
    {
        namewindow->hide();
        starter = new Starter();
        starter->opennewConnection(name->text());
        name->clear();




    }
    else
    {
        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText("This name is not contained in the database.\n\nMaybe you wrote it wrong ?\nTry creating a new contact...");
        msgbox.exec();
    }
    settings->endGroup();

}

StarterManager::~StarterManager()
{

}
