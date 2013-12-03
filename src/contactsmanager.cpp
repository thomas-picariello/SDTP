#include "contactsmanager.h"

ContactsManager::ContactsManager()
{

    Contactwindow = new QWidget();
    addcontactwindow = new QWidget();
    editcontactwindow = new QWidget();

    settings = new QSettings;

    contact = new Contact();

    addContact = new QPushButton("Add+");
    editContact = new QPushButton("Edit_");
    saveContact = new QPushButton("Save changes");

    menu = new QMenu();

    IP = new QLineEdit;
    port = new QLineEdit;
    key = new QLineEdit;
    name = new QLineEdit;

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    addclayout = new QVBoxLayout;
    addchlayout = new QHBoxLayout;
    addlabellayout = new QVBoxLayout;
    adddatalayout = new QVBoxLayout;

    label = new QLabel;
    addlabel = new QLabel;
    IPlabel = new QLabel;
    portlabel = new QLabel;
    keylabel = new QLabel;
    namelabel = new QLabel;

    menu->addAction(" /Under Construction\\");
    menu->addSeparator();
    menu->addAction("Just use Add function instead.");

    editContact->setMenu(menu);

    port->setInputMask("00009");
    port->setMaxLength(5);
    IP->setInputMask("009.009.009.009");
    name->setMaxLength(10);
    name->setInputMask("nnnnnnnnnn");

    label->setText("text to test the text capacity of QLabel");
    addlabel->setText("New Contact informations");
    IPlabel->setText("IP :");
    portlabel->setText("port :");
    namelabel->setText("name :");
    keylabel->setText("key :");

    adddatalayout->addWidget(name);
    adddatalayout->addWidget(IP);
    adddatalayout->addWidget(port);
    adddatalayout->addWidget(key);

    addlabellayout->addWidget(namelabel);
    addlabellayout->addWidget(IPlabel);
    addlabellayout->addWidget(portlabel);
    addlabellayout->addWidget(keylabel);

    addchlayout->addLayout(addlabellayout);
    addchlayout->addLayout(adddatalayout);

    addclayout->addWidget(addlabel);
    addclayout->addLayout(addchlayout);
    addclayout->addWidget(saveContact);



    vlayout->addWidget(addContact);
    vlayout->addWidget(editContact);

    hlayout->addLayout(vlayout);
    hlayout->addWidget(label);

    Contactwindow->setLayout(hlayout);
    addcontactwindow->setLayout(addclayout);



    connect(addContact  ,SIGNAL(clicked()),this,SLOT(addcontact()));
    connect(editContact ,SIGNAL(clicked()),this,SLOT(editcontact()));
    connect(saveContact ,SIGNAL(clicked()),this,SLOT(savecontact()));





}
void ContactsManager::contactsWindow()
{
    qDebug()<<"contactsWindow !";
    settings->beginGroup("Contacts/");
    qDebug()<<settings->childGroups();

    QStringList list;

    list = settings->childGroups();

    label->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));

    settings->endGroup();
    Contactwindow->show();


}
void ContactsManager::addcontact()
{
    name->setPlaceholderText("Name :");
    port->setPlaceholderText("80");
    IP->setPlaceholderText("127.0.0.1");
    key->setPlaceholderText("QWEQJBKQEQGE");

    qDebug()<<"addcontactwindow";
    addcontactwindow->show();





}
void ContactsManager::editcontact()
{


    addlabel->setText("Edit Contact informations");


    editcontactwindow->setLayout(addclayout);
    qDebug()<<"editcontactwindow";
    editcontactwindow->show();

}
void ContactsManager::savecontact()
{
    addcontactwindow->hide();

    qDebug()<<name->text();
    qDebug()<<IP->text();
    qDebug()<<port->text();
    qDebug()<<key->text();


    settings->setValue("Contacts/"+name->text()+"/name",name->text());
    settings->setValue("Contacts/"+name->text()+"/IP",IP->text());
    settings->setValue("Contacts/"+name->text()+"/port",port->text());
    settings->setValue("Contacts/"+name->text()+"/key",key->text());

    settings->beginGroup("Contacts/");
    QStringList list;
    list = settings->childGroups();
    label->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));
    settings->endGroup();

    name->clear();
    port->clear();
    IP->clear();
    key->clear();


}

ContactsManager::~ContactsManager(void)
{

}
