#include "contactsmanager.h"

ContactsManager::ContactsManager()
{

    m_contactManagerMainwindow = new QWidget();
    m_addContactWindow = new QWidget();
    m_editContactWindow = new QWidget();

    m_settings = new QSettings;

    m_contact = new Contact();

    m_addContactButton = new QPushButton("Add+");
    m_editContactButton = new QPushButton("Edit_");
    m_saveContactButton = new QPushButton("Save changes");

    menu = new QMenu();

    IP = new QLineEdit;
    port = new QLineEdit;
    key = new QLineEdit;
    name = new QLineEdit;

    m_addAndEditButtonLayout = new QVBoxLayout;
    m_contactManagerWindowMainLayout = new QHBoxLayout;
    m_addWindowMainLayout = new QVBoxLayout;
    m_addWindowCenterLayout = new QHBoxLayout;
    m_addWindowLabelLayout = new QVBoxLayout;
    m_addWindowLineEditLayout = new QVBoxLayout;

    m_contactListLabel = new QLabel;
    m_addlabel = new QLabel;
    m_IPlabel = new QLabel;
    m_portlabel = new QLabel;
    m_keylabel = new QLabel;
    m_namelabel = new QLabel;

    menu->addAction(" /Under Construction\\");
    menu->addSeparator();
    menu->addAction("Just use Add function instead.");

    m_editContactButton->setMenu(menu);

    port->setInputMask("00009");
    port->setMaxLength(5);
    IP->setInputMask("009.009.009.009");
    name->setMaxLength(10);
    name->setInputMask("nnnnnnnnnn");

    m_contactListLabel->setText("text to test the text capacity of QLabel");
    m_addlabel->setText("New Contact informations");
    m_IPlabel->setText("IP :");
    m_portlabel->setText("port :");
    m_namelabel->setText("name :");
    m_keylabel->setText("key :");

    m_addWindowLineEditLayout->addWidget(name);
    m_addWindowLineEditLayout->addWidget(IP);
    m_addWindowLineEditLayout->addWidget(port);
    m_addWindowLineEditLayout->addWidget(key);

    m_addWindowLabelLayout->addWidget(m_namelabel);
    m_addWindowLabelLayout->addWidget(m_IPlabel);
    m_addWindowLabelLayout->addWidget(m_portlabel);
    m_addWindowLabelLayout->addWidget(m_keylabel);

    m_addWindowCenterLayout->addLayout(m_addWindowLabelLayout);
    m_addWindowCenterLayout->addLayout(m_addWindowLineEditLayout);

    m_addWindowMainLayout->addWidget(m_addlabel);
    m_addWindowMainLayout->addLayout(m_addWindowCenterLayout);
    m_addWindowMainLayout->addWidget(m_saveContactButton);



    m_addAndEditButtonLayout->addWidget(m_addContactButton);
    m_addAndEditButtonLayout->addWidget(m_editContactButton);

    m_contactManagerWindowMainLayout->addLayout(m_addAndEditButtonLayout);
    m_contactManagerWindowMainLayout->addWidget(m_contactListLabel);

    m_contactManagerMainwindow->setLayout(m_contactManagerWindowMainLayout);
    m_addContactWindow->setLayout(m_addWindowMainLayout);



    connect(m_addContactButton  ,SIGNAL(clicked()),this,SLOT(addcontact()));
    connect(m_editContactButton ,SIGNAL(clicked()),this,SLOT(editcontact()));
    connect(m_saveContactButton ,SIGNAL(clicked()),this,SLOT(savecontact()));





}
void ContactsManager::contactsWindow()
{
    qDebug()<<"contactsWindow !";
    m_settings->beginGroup("Contacts/");
    qDebug()<<m_settings->childGroups();

    QStringList list;

    list = m_settings->childGroups();

    m_contactListLabel->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));

    m_settings->endGroup();
    m_contactManagerMainwindow->show();


}
void ContactsManager::addcontact()
{
    name->setPlaceholderText("Name :");
    port->setPlaceholderText("80");
    IP->setPlaceholderText("127.0.0.1");
    key->setPlaceholderText("QWEQJBKQEQGE");

    qDebug()<<"m_addContactWindow";
    m_addContactWindow->show();





}
void ContactsManager::editcontact()
{


    m_addlabel->setText("Edit Contact informations");


    m_editContactWindow->setLayout(m_addWindowMainLayout);
    qDebug()<<"m_editContactWindow";
    m_editContactWindow->show();

}
void ContactsManager::savecontact()
{
    m_addContactWindow->hide();

    QStringList list;

    m_contact->setname(name->text());
    m_contact->setIP(name->text(),IP->text());
    m_contact->setport(name->text(),port->text());
    m_contact->setkey(name->text(),key->text().toUtf8());

    list = m_contact->getnames();
    m_contactListLabel->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));

    name->clear();
    port->clear();
    IP->clear();
    key->clear();


}

ContactsManager::~ContactsManager(void)
{

}
