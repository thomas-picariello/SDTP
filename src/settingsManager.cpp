#include "settingsmanager.h"

SettingsManager::SettingsManager()
{
    setwindow = new QWidget();

    settings = new QSettings;

    labellayout = new QVBoxLayout;
    linelayout = new QVBoxLayout;
    slayout = new QHBoxLayout;

    portlabel = new QLabel;
    keylabel = new QLabel;

    portline = new QLineEdit;
    keyline = new QLineEdit;

    save = new QPushButton("Save");
    close = new QPushButton("Close");



    portlabel->setText("port to use :");
    keylabel->setText("key to use :");

    portline->setInputMask("00009");
    portline->setPlaceholderText("80");
    keyline->setMaxLength(2048);
    keyline->setPlaceholderText("QWERTZUIOPASDFGHJKLYXCVBNM");

    labellayout->addWidget(portlabel);
    labellayout->addWidget(keylabel);
    labellayout->addWidget(close);

    linelayout->addWidget(portline);
    linelayout->addWidget(keyline);
    linelayout->addWidget(save);

    slayout->addLayout(labellayout);
    slayout->addLayout(linelayout);

    setwindow->setLayout(slayout);



    connect(close,SIGNAL(clicked()),setwindow,SLOT(hide()));
    connect(save,SIGNAL(clicked()),this, SLOT(savesettings()));

}
void SettingsManager::settingsWindow()
{

    qDebug()<<"Test for SettingsWindow !";
    setwindow->show();

}
void SettingsManager::savesettings()
{
    setwindow->hide();

    settings->setValue("Settings/port",portline->text());
    settings->setValue("Settings/key",keyline->text());

    portline->clear();
    keyline->clear();



}

SettingsManager::~SettingsManager()
{

}
